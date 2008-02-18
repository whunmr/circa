import pdb

from Circa import (
  ca_types,
  ca_function
)

from Circa.builtin_functions import values

nextGlobalID = 1


class Term(object):
  def __init__(self, function, source_token=None):

    # initialize
    self.inputs = []
    self.function = function
    self.source_token = source_token
    self.users = set()
    self.pythonValue = None
    self.codeUnit = None

    # possibly make state
    self.state = function.makeState()

    # possibly create a branch
    self.branch = [] if function.hasBranch else None

    # possibly make training info
    if function.trainingType:
      self.training_info = function.trainingType()
      self.training_info.update(self)
    else: 
      self.training_info = None

    # assign a global ID
    global nextGlobalID
    self.globalID = nextGlobalID
    nextGlobalID += 1

  def getType(self):
    "Returns this term's output type"
    return self.function.outputType

  def inputsContain(self, term):
    "Returns True if our inputs contain the term"
    for input in self.inputs:
      if input == term: return True
    return false

  def changeFunction(self, new_func, initial_state=None):
    "Change this term's function"
    self.function = new_func

    if not initial_state:
      self.state = self.function.makeState()
    else:
      self.state = initial_state

  def pythonEvaluate(self):
    self.function.pythonEvaluate(self)

  def getIdentifier(self):
    return str(self.globalID)

  def printExtended(self, printer):
    printer.prints("%i: %s" % (self.globalID, self.function.name))

    if self.pythonValue:
      printer.prints(" " + str(self.pythonValue))

    printer.prints(" [")

    first_item = True
    for input in self.inputs:
      if not first_item: printer.prints(",")
      printer.prints(input.globalID)
      first_item = False
    printer.prints("]")

    printer.println()

    if self.state:
      label_branches = len(self.state.branches) > 1
      branch_index = -1

      for branch in self.state.branches:
        branch_index += 1

        # (maybe) label the branch index
        if label_branches:
          printer.indent(1)
          printer.println('Branch %i:' % branch_index)
          printer.indent(1)
        else:
          printer.indent(2)

        # print inner terms
        if branch.terms:
          for term in branch.terms:
            term.printExtended(printer)
        else:
          printer.println("(empty)")

        printer.unindent(2)

  def iterator(self, forwards=True):
    yield self
    if self.state:
      for branch in self.branches:
        for term in branch:
          yield term

  def equals(self, term):
    assert isinstance(term, Term)
    return self.pythonValue == term.pythonValue

  # value accessors
  def __int__(self):
    try: return int(self.pythonValue)
    except: return 0

  def __float__(self):
    try: return float(self.pythonValue)
    except: return 0.0

  def __str__(self):
    try: return str(self.pythonValue)
    except: return ""

  # member accessor
  def __getitem__(self, name):
    return self.state.getLocal(name)

import builtin_functions

def wrapNonTerm(obj):
  if not isinstance(obj, Term):
    return constant(obj)
  else:
    return obj

def placeholder():
  "Returns a new placeholder term"
  return Term(builtin_functions.PLACEHOLDER)

def constant(value, options={}):
  "Returns a constant term with the given value"
  type = ca_types.getTypeOfPythonObj(value)
  term = Term(values.Constant.fromType(type), **options)
  term.pythonValue = value
  return term

createConstant = constant

def variable(value, options={}):
  "Returns a variable term with the given value"
  type = ca_types.getTypeOfPythonObj(value)
  term = Term(values.Variable.fromType(type), **options)
  Variable.setValue(term, value)
  return term

createVariable = variable

def findExisting(function, inputs=[]):
  """
  Assuming you want a term that has the given function and inputs, this function
  returns such a function if it exists and if it makes sense (according to the function's
  definition) to allow people to reuse a function.
  """
  if function.shouldReuseExisting():
    # Try to find an existing term
    for input in inputs:
      for user_of_input in input.users:
        # Check if they are using the same function
        if user_of_input.function != function: continue

        # Check if all the inputs are the same
        def matches(pair):
          return pair[0].equals(pair[1])

        inputs_match = all(map(matches, zip(inputs, user_of_input.inputs)))

        # Todo: allow for functions that don't care what the function order is

        if not inputs_match: continue

        # Looks like this term is the same as what they want
        return user_of_input

  return None

 
