// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

// This file was generated using name_tool.py

#pragma once

namespace circa {

const int name_None = 0;
const int name_Invalid = 1;
const int name_File = 2;
const int name_Newline = 3;
const int name_Out = 4;
const int name_Unknown = 5;
const int name_Repeat = 6;
const int name_Success = 7;
const int name_Failure = 8;
const int name_EvaluationEmpty = 9;
const int name_HasEffects = 10;
const int name_Origin = 11;
const int name_FileNotFound = 12;
const int name_NotEnoughInputs = 13;
const int name_TooManyInputs = 14;
const int name_ExtraOutputNotFound = 15;
const int name_Default = 16;
const int name_ByDemand = 17;
const int name_Unevaluated = 18;
const int name_InProgress = 19;
const int name_Lazy = 20;
const int name_Consumed = 21;
const int name_Uncaptured = 22;
const int name_Return = 23;
const int name_Continue = 24;
const int name_Break = 25;
const int name_Discard = 26;
const int name_InfixOperator = 27;
const int name_FunctionName = 28;
const int name_TypeName = 29;
const int name_TermName = 30;
const int name_Keyword = 31;
const int name_Whitespace = 32;
const int name_UnknownIdentifier = 33;
const int name_LookupAny = 34;
const int name_LookupType = 35;
const int name_LookupFunction = 36;
const int name_LookupModule = 37;
const int name_Untyped = 38;
const int name_UniformListType = 39;
const int name_AnonStructType = 40;
const int name_StructType = 41;
const int name_NativePatch = 42;
const int name_PatchBlock = 43;
const int name_Bootstrapping = 44;
const int name_Done = 45;
const int name_StorageTypeNull = 46;
const int name_StorageTypeInt = 47;
const int name_StorageTypeFloat = 48;
const int name_StorageTypeBool = 49;
const int name_StorageTypeString = 50;
const int name_StorageTypeList = 51;
const int name_StorageTypeOpaquePointer = 52;
const int name_StorageTypeTerm = 53;
const int name_StorageTypeType = 54;
const int name_StorageTypeHandle = 55;
const int name_StorageTypeHashtable = 56;
const int name_StorageTypeObject = 57;
const int tok_Identifier = 58;
const int tok_Name = 59;
const int tok_Integer = 60;
const int tok_HexInteger = 61;
const int tok_Float = 62;
const int tok_String = 63;
const int tok_Color = 64;
const int tok_Bool = 65;
const int tok_LParen = 66;
const int tok_RParen = 67;
const int tok_LBrace = 68;
const int tok_RBrace = 69;
const int tok_LBracket = 70;
const int tok_RBracket = 71;
const int tok_Comma = 72;
const int tok_At = 73;
const int tok_Dot = 74;
const int tok_DotAt = 75;
const int tok_Star = 76;
const int tok_Question = 77;
const int tok_Slash = 78;
const int tok_DoubleSlash = 79;
const int tok_Plus = 80;
const int tok_Minus = 81;
const int tok_LThan = 82;
const int tok_LThanEq = 83;
const int tok_GThan = 84;
const int tok_GThanEq = 85;
const int tok_Percent = 86;
const int tok_Colon = 87;
const int tok_DoubleColon = 88;
const int tok_DoubleEquals = 89;
const int tok_NotEquals = 90;
const int tok_Equals = 91;
const int tok_PlusEquals = 92;
const int tok_MinusEquals = 93;
const int tok_StarEquals = 94;
const int tok_SlashEquals = 95;
const int tok_ColonEquals = 96;
const int tok_RightArrow = 97;
const int tok_LeftArrow = 98;
const int tok_Ampersand = 99;
const int tok_DoubleAmpersand = 100;
const int tok_DoubleVerticalBar = 101;
const int tok_Semicolon = 102;
const int tok_TwoDots = 103;
const int tok_Ellipsis = 104;
const int tok_TripleLThan = 105;
const int tok_TripleGThan = 106;
const int tok_Pound = 107;
const int tok_Def = 108;
const int tok_Type = 109;
const int tok_UnusedName1 = 110;
const int tok_UnusedName2 = 111;
const int tok_UnusedName3 = 112;
const int tok_If = 113;
const int tok_Else = 114;
const int tok_Elif = 115;
const int tok_For = 116;
const int tok_State = 117;
const int tok_Return = 118;
const int tok_In = 119;
const int tok_True = 120;
const int tok_False = 121;
const int tok_Namespace = 122;
const int tok_Include = 123;
const int tok_And = 124;
const int tok_Or = 125;
const int tok_Not = 126;
const int tok_Discard = 127;
const int tok_Null = 128;
const int tok_Break = 129;
const int tok_Continue = 130;
const int tok_Switch = 131;
const int tok_Case = 132;
const int tok_While = 133;
const int tok_Require = 134;
const int tok_Package = 135;
const int tok_Section = 136;
const int tok_Whitespace = 137;
const int tok_Newline = 138;
const int tok_Comment = 139;
const int tok_Eof = 140;
const int tok_Unrecognized = 141;
const int op_NoOp = 142;
const int op_Pause = 143;
const int op_SetNull = 144;
const int op_InlineCopy = 145;
const int op_CallBlock = 146;
const int op_DynamicCall = 147;
const int op_ClosureCall = 148;
const int op_FireNative = 149;
const int op_CaseBlock = 150;
const int op_ForLoop = 151;
const int op_ExitPoint = 152;
const int op_FinishFrame = 153;
const int op_FinishLoop = 154;
const int op_ErrorNotEnoughInputs = 155;
const int op_ErrorTooManyInputs = 156;
const int name_LoopProduceOutput = 157;
const int name_FlatOutputs = 158;
const int name_OutputsToList = 159;
const int name_Multiple = 160;
const int name_Cast = 161;
const int name_DynamicMethodOutput = 162;
const int name_FirstStatIndex = 163;
const int stat_TermsCreated = 164;
const int stat_TermPropAdded = 165;
const int stat_TermPropAccess = 166;
const int stat_InternedNameLookup = 167;
const int stat_InternedNameCreate = 168;
const int stat_Copy_PushedInputNewFrame = 169;
const int stat_Copy_PushedInputMultiNewFrame = 170;
const int stat_Copy_PushFrameWithInputs = 171;
const int stat_Copy_ListDuplicate = 172;
const int stat_Copy_LoopCopyRebound = 173;
const int stat_Cast_ListCastElement = 174;
const int stat_Cast_PushFrameWithInputs = 175;
const int stat_Cast_FinishFrame = 176;
const int stat_Touch_ListCast = 177;
const int stat_ValueCreates = 178;
const int stat_ValueCopies = 179;
const int stat_ValueCast = 180;
const int stat_ValueCastDispatched = 181;
const int stat_ValueTouch = 182;
const int stat_ListsCreated = 183;
const int stat_ListsGrown = 184;
const int stat_ListSoftCopy = 185;
const int stat_ListHardCopy = 186;
const int stat_DictHardCopy = 187;
const int stat_StringCreate = 188;
const int stat_StringDuplicate = 189;
const int stat_StringResizeInPlace = 190;
const int stat_StringResizeCreate = 191;
const int stat_StringSoftCopy = 192;
const int stat_StringToStd = 193;
const int stat_StepInterpreter = 194;
const int stat_InterpreterCastOutputFromFinishedFrame = 195;
const int stat_BlockNameLookups = 196;
const int stat_PushFrame = 197;
const int stat_LoopFinishIteration = 198;
const int stat_LoopWriteOutput = 199;
const int stat_WriteTermBytecode = 200;
const int stat_DynamicCall = 201;
const int stat_FinishDynamicCall = 202;
const int stat_DynamicMethodCall = 203;
const int stat_SetIndex = 204;
const int stat_SetField = 205;
const int name_LastStatIndex = 206;
const int name_LastBuiltinName = 207;

const char* builtin_name_to_string(int name);
int builtin_name_from_string(const char* str);

} // namespace circa
