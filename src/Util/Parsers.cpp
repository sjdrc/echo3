
#include <echo/Util/Parsers.h>
#include <echo/Util/Utils.h>
#include <echo/Util/FunctionBinder.h>

namespace Echo
{
	namespace Parser
	{
		Parser::Parser() : mOutputErrors(true){}
		Parser::~Parser(){}
		
		void Parser::ReportError(std::string error) const
		{
			if(mOutputErrors)
			{
				ECHO_LOG_ERROR(error);
			}
			mLastParseErrors.push_back(error);
		}

		CalculatorWithVariables::CalculatorWithVariables(shared_ptr<FunctionBinder> binder) : CalculatorWithVariables::base_type(expression), mBinder(binder)
		{
			Initialise();
		}

		/**
		 * Default constructor that creates a function binder suitable for parsing.
		 */
		CalculatorWithVariables::CalculatorWithVariables() : CalculatorWithVariables::base_type(expression), mBinder(new FunctionBinder())
		{
			//Change function processing to just strings, this also prevents parameters.
			mBinder->SetFunctionRegEx("(.*)()");
			Initialise();
		}

		CalculatorWithVariables::CalculatorWithVariables(const CalculatorWithVariables& other) : CalculatorWithVariables::base_type(expression), mBinder(other.mBinder)
		{
			Initialise();
		}

		CalculatorWithVariables& CalculatorWithVariables::operator=(const CalculatorWithVariables& other)
		{
			mBinder = other.mBinder;
			return *this;
		}

		void CalculatorWithVariables::RegisterVariableViaGetter(const std::string& name, function<double()> getterFunction)
		{
			mBinder->RegisterVoid(name,getterFunction);
		}

		void CalculatorWithVariables::DeregisterVariable(const std::string& name)
		{
			mBinder->Deregister(name);
		}

		/**
		 * Parse a string expression.
		 * @param expression The expression to parse.
		 * @return If the expression is successfully parsed the result, otherwise the object
		 * returned will be false when converted to a bool.
		 */
		optional<double> CalculatorWithVariables::Parse(const std::string& expression) const
		{
			using boost::spirit::ascii::space;
			mLastParseErrors.resize(0);
			double result;
			std::string::const_iterator it = expression.begin();
			std::string::const_iterator end = expression.end();
			bool r = phrase_parse(it, end, *this, space, result);

			if (r && it == end && GetLastParseErrors().empty())
			{
				return result;
			}else
			{
				std::stringstream ss;
				ss << "Parsing of expression: \""<<expression<<"\" failed at \"" << std::string(it, end) << "\"";
				ReportError(ss.str());
			}
			return none;
		}

		/**
		 * Internal method called to get a value via the function binder.
		 * The symbol used in an expression is used as the function.
		 * @param symbol The name of the variable.
		 * @return 
		 */
		double CalculatorWithVariables::GetValue(const std::string& symbol)
		{
			if(mBinder)
			{
				double result = 1;
				FunctionBinder::CallResult callResult = mBinder->Call<double>(symbol,&result);
				if(callResult.mStatus==FunctionBinder::CallStatuses::SUCCESS)
				{
					return result;
				}else
				{
					std::stringstream ss;
					ss << "Symbol \"" << symbol << "\" look up failed";
					ReportError(ss.str());
				}
			}
			return 0;
		}

		inline double CalculatorWithVariables::Modulus(int x, int y)
		{
			return x%y;
		}

		void CalculatorWithVariables::Initialise()
		{
			assert(mBinder);

			using qi::_val;
			using qi::_1;
			using qi::uint_;
			using qi::double_;
			using ascii::char_;
			using ascii::alpha;
			using ascii::space;
			using qi::as_string;
			using qi::lexeme;


			expression =
				term                            [_val = _1]
				>> *(   ('+' >> term            [_val += _1])
					|   ('-' >> term            [_val -= _1])
					)
				;

			term =
				factor                          [_val = _1]
				>> *(   ('*' >> factor          [_val *= _1])
					|   ('/' >> factor          [_val /= _1])
					|   ('%' >> factor          [(_val = boost::phoenix::bind(&CalculatorWithVariables::Modulus, this, _val, qi::_1))])
					)
				;

			//variable = char_[_val=1];
			variable = (as_string[(lexeme[ +(char_ - char_("+-*/()%=><!|&") - space) ])])[_val = boost::phoenix::bind(&CalculatorWithVariables::GetValue, this, qi::_1)];

			factor =
				double_                         [_val = _1]
				| variable						[_val = _1]
				|   '(' >> expression           [_val = _1] >> ')'
				|   ('-' >> factor              [_val = -_1])
				|   ('+' >> factor              [_val = _1])
				;
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////
		
		ConditionalEvaluator::ConditionalEvaluator(shared_ptr<FunctionBinder> binder) : ConditionalEvaluator::base_type(compoundCondition), mBinder(binder)
		{
			Initialise();
		}

		/**
		 * Default constructor that creates a function binder suitable for parsing.
		 */
		ConditionalEvaluator::ConditionalEvaluator() : ConditionalEvaluator::base_type(compoundCondition), mBinder(new FunctionBinder())
		{
			//Change function processing to just strings, this also prevents parameters.
			mBinder->SetFunctionRegEx("(.*)()");
			Initialise();
		}

		void ConditionalEvaluator::RegisterVariableViaGetter(const std::string& name, function<double()> getterFunction)
		{
			mBinder->RegisterVoid(name,getterFunction);
		}

		void ConditionalEvaluator::DeregisterVariable(const std::string& name)
		{
			mBinder->Deregister(name);
		}

		/**
		 * Parse a string expression.
		 * @param expression The expression to parse.
		 * @return If the expression is successfully parsed the result, otherwise the object
		 * returned will be false when converted to a bool.
		 */
		optional<bool> ConditionalEvaluator::Parse(const std::string& expression) const
		{
			using boost::spirit::ascii::space;
			mLastParseErrors.resize(0);
			bool result;
			std::string::const_iterator it = expression.begin();
			std::string::const_iterator end = expression.end();
			bool r = phrase_parse(it, end, *this, space, result);

			if (r && it == end && GetLastParseErrors().empty())
			{
				return result;
			}else
			{
				std::stringstream ss;
				ss << "Parsing of expression: \""<<expression<<"\" failed at \"" << std::string(it, end) << "\"";
				ReportError(ss.str());
			}
			return none;
		}

		/**
		 * Internal method called to get a value via the function binder.
		 * The symbol used in an expression is used as the function.
		 * @param symbol The name of the variable.
		 * @return 
		 */
		double ConditionalEvaluator::GetValue(const std::string& symbol)
		{
			if(mBinder)
			{
				double result = 1;
				FunctionBinder::CallResult callResult = mBinder->Call<double>(symbol,&result);
				if(callResult.mStatus==FunctionBinder::CallStatuses::SUCCESS)
				{
					return result;
				}else
				{
					std::stringstream ss;
					ss << "Symbol \"" << symbol << "\" look up failed";
					ReportError(ss.str());
				}
			}
			return 0;
		}

		inline double ConditionalEvaluator::Modulus(int x, int y)
		{
			return x%y;
		}

		void ConditionalEvaluator::Initialise()
		{
			assert(mBinder);

			using qi::_val;
			using qi::_1;
			using qi::_a;
			using qi::uint_;
			using qi::double_;
			using ascii::char_;
			using ascii::alpha;
			using ascii::space;
			using qi::as_string;
			using qi::lexeme;
			using qi::lit;
			
			expression =
				term                            [_val = _1]
				>> *(   ('+' >> term            [_val += _1])
					|   ('-' >> term            [_val -= _1])
					)
				;

			term =
				factor                          [_val = _1]
				>> *(   ('*' >> factor          [_val *= _1])
					|   ('/' >> factor          [_val /= _1])
					|   ('%' >> factor          [(_val = boost::phoenix::bind(&ConditionalEvaluator::Modulus, this, _val, qi::_1))])
					)
				;

			variable = (as_string[(lexeme[ +(char_ - char_("+-*/()%=><!|&") - space) ])])[_val = boost::phoenix::bind(&ConditionalEvaluator::GetValue, this, qi::_1)];

			factor =
				double_                         [_val = _1]
				| variable						[_val = _1]
				|   '(' >> expression           [_val = _1] >> ')'
				|   ('-' >> factor              [_val = -_1])
				|   ('+' >> factor              [_val = _1])
				;
			
			condition =
				(expression [_a = _1]
					>> (	('>' >> expression[_val = _a > _1])
						|	('<' >> expression[_val = _a < _1])
						|	(lit("==") >> expression[_val = _a == _1])
						|	(lit("!=") >> expression[_val = _a != _1])
						|	(lit(">=") >> expression[_val = _a >= _1])
						|	(lit("<=") >> expression[_val = _a <= _1])
						))
				| expression[_val = _1];						//Evaluate the result of the expression to bool.

			compoundCondition =
				(condition[_val = _1]
					>> *(   (lit("&&") >> condition[_val = _val && _1])
						|   (lit("||") >> condition[_val = _val || _1])
						)
				);
		}
		
		////////////////////////////////////////////////////////////////////////
		// VariableSubstitutor
		
		VariableSubstitutor::VariableSubstitutor(shared_ptr<FunctionBinder> binder) : mBinder(binder)
		{
		}

		VariableSubstitutor::VariableSubstitutor() : mBinder(new FunctionBinder())
		{
			//Change function processing to just strings, this also prevents parameters.
			mBinder->SetFunctionRegEx("(.*)()");
		}

		VariableSubstitutor::VariableSubstitutor(const VariableSubstitutor& other) : mBinder(other.mBinder)
		{
		}

		VariableSubstitutor& VariableSubstitutor::operator=(const VariableSubstitutor& other)
		{
			mBinder = other.mBinder;
			return *this;
		}

		void VariableSubstitutor::RegisterVariableViaGetter(const std::string& name, function<std::string()> getterFunction)
		{
			mBinder->RegisterVoid(name,getterFunction);
		}

		void VariableSubstitutor::DeregisterVariable(const std::string& name)
		{
			mBinder->Deregister(name);
		}

		std::string VariableSubstitutor::Parse(std::string expression) const
		{
			boost::regex variableExpression(R"EOF(\\?\$([^\s\$\[\]\(\)\.\-\*\^\\\/\"\+{},?!<>=:;|&%#@`~]+))EOF");

			boost::sregex_token_iterator iter(expression.begin(), expression.end(), variableExpression, 0);
			boost::sregex_token_iterator end;

			std::string parsed;
			std::string::const_iterator lastEndOfVariable = expression.begin();
			for( ; iter != end; ++iter )
			{
				std::string nonvar;
				parsed += nonvar.assign(lastEndOfVariable,iter->first);
				lastEndOfVariable = iter->second;
				std::string variable(iter->first, iter->second);
				if(variable.size()>1 && variable[0]=='\\')
				{
					// This was "\$SOMETHING" so we want to output $SOMETHING
					parsed += variable.substr(1);
				}else
				{
					// This was "$SOMETHING" so we want to use "SOMETHING" as a variable to get a value
					parsed += GetValue(variable.substr(1));
				}
			}
			// Append the last non-variable part of the string
			std::string nonvar;
			std::string::const_iterator endIt = expression.end();
			parsed += nonvar.assign(lastEndOfVariable,endIt);
			mLastParseErrors.resize(0);
			return parsed;
		}

		std::string VariableSubstitutor::GetValue(const std::string& symbol) const
		{
			if(mBinder)
			{
				std::string result = "";
				FunctionBinder::CallResult callResult = mBinder->Call<std::string>(symbol,&result);
				if(callResult.mStatus==FunctionBinder::CallStatuses::SUCCESS)
				{
					return result;
				}else
				{
					std::stringstream ss;
					ss << "Symbol \"" << symbol << "\" look up failed";
					ReportError(ss.str());
				}
			}
			return "";
		}		
	}
}
