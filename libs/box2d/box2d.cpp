// Copyright (c) Paul Hodge. See LICENSE file for license terms.

#include "Box2D/Box2D.h"

#include "circa.h"
#include "gc.h"
#include "list_shared.h"
#include "types/handle.h"

using namespace circa;

namespace box2d_support {

#define LOG_HANDLE_CREATION 0

b2World *g_world = NULL;

void initialize_world()
{
    if (g_world == NULL)
        g_world = new b2World(b2Vec2(), false);
}

float c_timestep = 1.0/60.0;
int c_velocityIterations = 6;
int c_positionIterations = 2;

// Scale between screen & world coordinates. Box2d says that it likes objects
// to have a size between 0.1 and 10 units.
float screen_to_world(float screen) { return screen / 10.0; }
float world_to_screen(float world) { return world * 10.0; }

// Circa angles are in the range of 0..1
float radians_to_unit_angles(float radians) { return radians / M_PI; }
float unit_angles_to_radians(float unit) { return unit * M_PI; }

void b2Vec2_to_point(b2Vec2 const& vec, TaggedValue* point)
{
    set_point(point, world_to_screen(vec.y), world_to_screen(vec.y));
}

b2Vec2 point_to_b2Vec2(TaggedValue* point)
{
    float x,y;
    get_point(point, &x, &y);
    return b2Vec2(screen_to_world(x), screen_to_world(y));
}

// The BodyHandle type is used to hold on to b2Body objects.
Type g_bodyHandle_t;

GcHeap g_gcHeap;

struct BodyHandle
{
    GcHeader gcHeader;
    b2Body* body;
    ListData* containingList;
    int globalIndex;

    BodyHandle() : body(NULL), containingList(NULL)
    {
        initialize_new_gc_object(&g_gcHeap, &gcHeader);
    }

    ~BodyHandle() {
        if (g_world == NULL)
            return;
        #if LOG_HANDLE_CREATION
        std::cout << "Destroyed " << body << std::endl;
        #endif
        g_world->DestroyBody(body);
        remove_gc_object(&g_gcHeap, &gcHeader);
    }
};

b2Body* get_body_from_handle(TaggedValue* value)
{
    BodyHandle* bodyHandle = (BodyHandle*) handle_t::get_ptr(value);
    if (bodyHandle == NULL)
        return NULL;
    return bodyHandle->body;
}

bool is_valid_body_handle(TaggedValue* value)
{
    return value->value_type == &g_bodyHandle_t
        && get_body_from_handle(value) != NULL;
}

CA_FUNCTION(step)
{
    initialize_world();

    g_world->Step(c_timestep, c_velocityIterations, c_positionIterations);
    g_world->ClearForces();
}

CA_FUNCTION(gravity)
{
    initialize_world();

    Point& gravity = *Point::checkCast(INPUT(0));
    g_world->SetGravity(b2Vec2(gravity.getX(), gravity.getY()));
}

CA_FUNCTION(create_body)
{
    // Inputs:
    //   int bodyType
    //   Point initialPosition
    //   number initialRotation
    //
    // Outputs:
    //   BodyPtr handle
    
    initialize_world();

    int bodyType = INT_INPUT(0);

    Point& initialPosition = *Point::checkCast(INPUT(1));
    float initialRotation = as_float(INPUT(2));

    // Create a b2Body
    b2BodyDef bodyDef;

    switch (bodyType) {
    case 0: bodyDef.type = b2_staticBody; break;
    case 1: bodyDef.type = b2_dynamicBody; break;
    case 2: bodyDef.type = b2_kinematicBody; break;
    }
    
    bodyDef.position.Set(
        screen_to_world(initialPosition.getX()),
        screen_to_world(initialPosition.getY()));

    bodyDef.angle = unit_angles_to_radians(initialRotation);

    BodyHandle* bodyHandle = handle_t::create<BodyHandle>(OUTPUT, &g_bodyHandle_t);
    bodyHandle->containingList = (ListData*) OUTPUT->value_data.ptr;

    bodyHandle->body = g_world->CreateBody(&bodyDef);

    #if LOG_HANDLE_CREATION
    std::cout << "Created " << bodyHandle->body << std::endl;
    #endif
}

CA_FUNCTION(set_body_fixtures)
{
    // Inputs:
    //   BodyPtr handle
    //   List fixtureDefs:
    //     int type
    //     either int or Point - type-specific data
    //     float density
    //     float friction
    //     float restitution
    b2Body* body = get_body_from_handle(INPUT(0));

    if (body == NULL)
        return error_occurred(CONTEXT, CALLER, "invalid body handle");

    // Remove any old fixtures
    while (body->GetFixtureList())
        body->DestroyFixture(body->GetFixtureList());

    // Append new fixtures according to the fixtureDefs list
    List& fixtureDefs = *List::checkCast(INPUT(1));

    for (int i=0; i < fixtureDefs.length(); i++) {

        List& fixtureDefIn = *List::checkCast(fixtureDefs[i]);

        int shapeType = as_int(fixtureDefIn[0]);

        b2FixtureDef fixtureDef;
        b2PolygonShape polygonShape;
        b2CircleShape circleShape;

        if (shapeType == 0) {
            Point& size = *Point::checkCast(fixtureDefIn[1]);
            polygonShape.SetAsBox(
                screen_to_world(size.getX()),
                screen_to_world(size.getY()));
            fixtureDef.shape = &polygonShape;
        } else if (shapeType == 1) {
            float radius = to_float(fixtureDefIn[1]);
            circleShape.m_radius = screen_to_world(radius);
            fixtureDef.shape = &circleShape;
        }

        fixtureDef.density = to_float(fixtureDefIn[2]);
        fixtureDef.friction = to_float(fixtureDefIn[3]);
        fixtureDef.restitution = to_float(fixtureDefIn[4]);

        body->CreateFixture(&fixtureDef);
    }
}

CA_FUNCTION(get_body_points)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;
    
    List& points = *List::cast(OUTPUT, 0);

    // Copy vertex locations
    for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        b2Shape* shape = fixture->GetShape();

        if (shape->GetType() == b2Shape::e_polygon) {
            b2PolygonShape *poly = (b2PolygonShape*) shape;

            int count = poly->GetVertexCount();

            for (int i = 0; i < count; i++) {
                b2Vec2 vec = body->GetWorldPoint(poly->GetVertex(i));
                Point* p = Point::cast(points.append());
                p->set(
                    world_to_screen(vec.x),
                    world_to_screen(vec.y));
            }
        }
    }
}

CA_FUNCTION(get_body_position)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;

    b2Vec2_to_point(body->GetPosition(), OUTPUT);
}

CA_FUNCTION(set_body_position)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;

    body->SetTransform(point_to_b2Vec2(INPUT(1)), body->GetAngle());
}

CA_FUNCTION(get_body_rotation)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;

    set_float(OUTPUT, radians_to_unit_angles(body->GetAngle()));
}


CA_FUNCTION(set_body_rotation)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;

    float rotation = unit_angles_to_radians(FLOAT_INPUT(1));

    body->SetTransform(body->GetPosition(), rotation);
}

CA_FUNCTION(get_linear_velocity)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;

    b2Vec2_to_point(body->GetLinearVelocity(), OUTPUT);
}

CA_FUNCTION(set_linear_velocity)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;

    std::cout << "set_linear_velocity" << INPUT(1)->toString() << std::endl;

    body->SetLinearVelocity(point_to_b2Vec2(INPUT(1)));
}

CA_FUNCTION(apply_force)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;

    body->ApplyForce(point_to_b2Vec2(INPUT(1)), point_to_b2Vec2(INPUT(2)));
}
CA_FUNCTION(apply_torque)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;

    body->ApplyTorque(FLOAT_INPUT(1));
}
CA_FUNCTION(apply_linear_impulse)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;

    body->ApplyLinearImpulse(point_to_b2Vec2(INPUT(1)), point_to_b2Vec2(INPUT(2)));
}
CA_FUNCTION(apply_angular_impulse)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;

    body->ApplyAngularImpulse(FLOAT_INPUT(1));
}

CA_FUNCTION(body_contains_point)
{
    b2Body* body = get_body_from_handle(INPUT(0));
    if (body == NULL)
        return;

    Point& input = *Point::checkCast(INPUT(1));

    b2Vec2 p(
        screen_to_world(input.getX()),
        screen_to_world(input.getY()));

    b2Fixture* fixture = body->GetFixtureList();

    while (fixture) {
        if (fixture->TestPoint(p)) {
            set_bool(OUTPUT, true);
            return;
        }
        fixture = fixture->GetNext();
    }

    set_bool(OUTPUT, false);
}

void run_global_refcount_check()
{
    std::cout << "run_global_refcount_check" << std::endl;

    TaggedValue evalContext;
    TaggedValue usersBranch;
    TaggedValue runtimeBranch;
        
    app::App* app = &app::get_global_app();
    set_transient_value(&evalContext, &app->_evalContext, &EVAL_CONTEXT_T);
    set_transient_value(&usersBranch, app->_usersBranch, &BRANCH_T);
    set_transient_value(&runtimeBranch, app->_runtimeBranch, &BRANCH_T);

    //recursive_dump_heap(&evalContext, "evalContext");
    //recursive_dump_heap(&usersBranch, "usersBranch");
    //recursive_dump_heap(&runtimeBranch, "runtimeBranch");

    BodyHandle* handle = (BodyHandle*) g_gcHeap.firstObject;
    while (handle != NULL) {

        std::cout << "looking at " << handle->containingList
            << " with refcount " << handle->containingList->refCount
            << std::endl;

        List references;
        list_references_to_pointer(&evalContext, handle, &references);
        list_references_to_pointer(&usersBranch, handle, &references);
        list_references_to_pointer(&runtimeBranch, handle, &references);

        for (int i=0; i < references.length(); i++)
            std::cout << references[i]->asString() << std::endl;

        handle = (BodyHandle*) handle->gcHeader.next;
    }

    cleanup_transient_value(&evalContext);
    cleanup_transient_value(&usersBranch);
    cleanup_transient_value(&runtimeBranch);
}

void on_frame_callback(void* userdata, app::App* app, int step)
{
    //run_global_refcount_check();
    //std::cout << "post_frame: " << app->_evalContext.state.toString() << std::endl;
}

void setup(Branch& kernel)
{
    handle_t::setup_type<BodyHandle>(&g_bodyHandle_t);
    g_bodyHandle_t.name = "Box2d::BodyHandle";

    Branch& ns = kernel["box2d"]->nestedContents;

    install_function(ns["step"], step);
    install_function(ns["gravity"], gravity);
    install_function(ns["create_body"], create_body);
    install_function(ns["set_body_fixtures"], set_body_fixtures);
    install_function(ns["get_body_points"], get_body_points);
    install_function(ns["get_body_position"], get_body_position);
    install_function(ns["get_body_rotation"], get_body_rotation);
    install_function(ns["set_body_position"], set_body_position);
    install_function(ns["set_body_rotation"], set_body_rotation);
    install_function(ns["get_linear_velocity"], get_linear_velocity);
    install_function(ns["set_linear_velocity"], set_linear_velocity);
    install_function(ns["apply_torque"], apply_torque);
    install_function(ns["apply_linear_impulse"], apply_linear_impulse);
    install_function(ns["apply_angular_impulse"], apply_angular_impulse);
    install_function(ns["body_contains_point"], body_contains_point);

    //app::get_global_app().addPreFrameCallback(on_frame_callback, NULL);
    app::get_global_app().addPostFrameCallback(on_frame_callback, NULL);
}

} // box2d_support
