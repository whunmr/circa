// Copyright 2008 Paul Hodge

#ifndef CIRCA_TERM_SYNTAX_HINTS_INCLUDED
#define CIRCA_TERM_SYNTAX_HINTS_INCLUDED

#include "common_headers.h"

namespace circa {

struct TermSyntaxHints
{
    struct InputSyntax {
        enum Style {
            UNKNOWN_STYLE=0,
            BY_NAME,
            BY_SOURCE };

        Style style;
        std::string name;

        InputSyntax() : style(UNKNOWN_STYLE) {}
    };

    enum DeclarationStyle {
        UNKNOWN_DECLARATION_STYLE=0,
        FUNCTION_CALL,
        INFIX,
        DOT_CONCATENATION,
        LITERAL_VALUE
    };

    // Members:
    int line;
    int startChar;
    int endChar;
    DeclarationStyle declarationStyle;
    std::vector<InputSyntax> inputSyntax;
    bool occursInsideAnExpression;
    std::string functionName;
    std::string precedingWhitespace;
    std::string followingWhitespace;

    TermSyntaxHints() :
        line(0),
        startChar(0),
        endChar(0),
        declarationStyle(UNKNOWN_DECLARATION_STYLE),
        occursInsideAnExpression(false)
    {}

    InputSyntax getInputSyntax(unsigned int index) {
        if (index >= inputSyntax.size())
            return InputSyntax();

        return inputSyntax[index];
    }

    void setInputSyntax(unsigned int index, InputSyntax const& syntax)
    {
        assert(index < 1000);

        while (index >= inputSyntax.size())
            inputSyntax.push_back(InputSyntax());

        inputSyntax[index] = syntax;
    }
};

}

#endif
