// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "circa/circa.h"

#include "list.h"
#include "names.h"
#include "parser.h"
#include "string_type.h"
#include "tagged_value.h"
#include "token.h"
#include "type.h"

namespace circa {

struct ParseContext
{
    TokenStream* tokens;
    caValue* out;
};

static void drop_whitespace(TokenStream* tokens)
{
    while (tokens->nextIs(tok_Whitespace))
        tokens->consume();
}

static void parse_value(TokenStream* tokens, caValue* out)
{
    // ignore leading whitespace
    drop_whitespace(tokens);

    if (tokens->finished()) {
        set_string(out, "unexpected end of string");

    } else if (tokens->nextIs(tok_Integer)) {
        set_int(out, atoi(tokens->nextStr().c_str()));
        tokens->consume(tok_Integer);
    } else if (tokens->nextIs(tok_Float)) {
        set_float(out, (float) atof(tokens->nextStr().c_str()));
        tokens->consume(tok_Float);
    } else if (tokens->nextIs(tok_String)) {
        std::string s = tokens->nextStr();
        parser::unquote_and_unescape_string(s.c_str(), out);
        tokens->consume(tok_String);
    } else if (tokens->nextIs(tok_LBracket)) {
        tokens->consume(tok_LBracket);
        drop_whitespace(tokens);

        set_list(out, 0);

        while (!tokens->nextIs(tok_RBracket) && !tokens->finished()) {
            caValue* element = list_append(out);
            parse_value(tokens, element);

            if (tokens->nextIs(tok_Comma))
                tokens->consume(tok_Comma);
            drop_whitespace(tokens);
        }

        if (!tokens->finished())
            tokens->consume(tok_RBracket);
    } else if (tokens->nextIs(tok_True)) {
        set_bool(out, true);
        tokens->consume();
    } else if (tokens->nextIs(tok_False)) {
        set_bool(out, false);
        tokens->consume();
    } else if (tokens->nextIs(tok_Minus)) {
        tokens->consume(tok_Minus);

        parse_value(tokens, out);

        if (is_int(out)) {
            set_int(out, -1 * as_int(out));
        } else if (is_float(out)) {
            set_float(out, -1 * as_float(out));
        } else {
            set_string(out, "error, minus sign must preceed number");
        }

    } else {
        set_string(out, "unrecognized token: ");
        string_append(out, tokens->nextStr().c_str());
        tokens->consume();
    }

    // ignore trailing whitespace
    drop_whitespace(tokens);
}

void parse_string_repr(const char* str, caValue* out)
{
    TokenStream tokens(str);
    parse_value(&tokens, out);
}

void write_string_repr(caValue* value, caValue* out)
{
    // For certain types, just use to_string
    if (is_int(value) || is_float(value) || is_bool(value)) {
        set_string(out, to_string(value).c_str());
    } else if (is_list(value)) {
        set_string(out, "");
        string_append(out, "[");

        for (int i=0; i < list_length(value); i++) {
            if (i > 0)
                string_append(out, ", ");

            Value elementStr;
            write_string_repr(list_get(value, i), &elementStr);
            string_append(out, &elementStr);
        }

        string_append(out, "]");
    } else if (is_string(value)) {
        parser::quote_and_escape_string(as_cstring(value), out);
    } else {
        set_string(out, "error: no string repr for type ");
        string_append(out, as_cstring(&value->value_type->name));
    }
}

} // namespace circa
