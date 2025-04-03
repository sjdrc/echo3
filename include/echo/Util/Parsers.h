#ifndef PARSERS_H
#define PARSERS_H

#include <echo/Types.h>
#include <echo/Util/Utils.h>

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix.hpp>
#include <echo/cpp/functional>

#include <vector>

namespace Echo
{
	class FunctionBinder;
	
	namespace Parser
	{
		class Parser
		{
		public:
			Parser();
			~Parser();
			
			/**
			 * Set whether errors will be logged or not.
			 * The last parse errors can be obtained with GetLastParseErrors() either way.
			 */
			void SetOutputErrors(bool outputErrors) {mOutputErrors=outputErrors;}

			/**
			 * Get whether or not errors will logged.
			 */
			bool GetOutputErrors() const {return mOutputErrors;}
			
			/**
			 * Get the last parse errors.
			 */
			const std::vector<std::string>& GetLastParseErrors() const {return mLastParseErrors;}
		protected:
			
			/**
			 * Method to allow parser implementations to report errors.
			 * @note this method is const and the error vector is mutable since the Parse() method
			 * needs to be const. This isn't ideal but currently what we have to deal with due to
			 * some of the parsers setting up rules for parsing during initialisation that include
			 * using an accessor on the instance, so we cannot directly pass a vector as a parameter
			 * to populat with errors.
			 * @param error the error string.
			 */
			void ReportError(std::string error) const;
			bool mOutputErrors;
			mutable std::vector< std::string > mLastParseErrors;
		};

		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;

		/**
		 * Parser that evaluates expressions and has access to variables from a function binder.
		 * This parser will take an expression such as 1 + 2 or 2 + 3 * 10 and return the result.
		 * 
		 * Values are processed as doubles.
		 * 
		 * This parser supports registration of variables which are given a name and can be used within
		 * the expression.
		 * 
		 * For example:
		 * 
		 *		Scalar mass = 120;
		 *		CalculatorWithVariables parser;
		 *		parser.RegisterVariable("mass",mass);
		 *		optional<double> result = parser("mass + 2 * 3");
		 *		if(result)
		 *		{
		 *			// Use *result, evaluates to 126 as mass evaluates to 120
		 *		}
		 * 
		 * You can also group expression terms in brackets.
		 * 
		 * Operator precedence is
		 *	* / %
		 *	+ -
		 * @note: When using % values are converted to integers first.
		 */
		struct CalculatorWithVariables : qi::grammar<std::string::const_iterator, double(), ascii::space_type>, public Parser
		{
		public:
			/**
			 * Construct with a custom FunctionBinder.
			 * @param binder A pre-configured function binder.
			 */
			CalculatorWithVariables(shared_ptr<FunctionBinder> binder);

			/**
			 * Default constructor that creates a function binder suitable for parsing.
			 */
			CalculatorWithVariables();

			CalculatorWithVariables(const CalculatorWithVariables& other);
			CalculatorWithVariables& operator=(const CalculatorWithVariables& other);

			/**
			 * Register a variable for the expression that uses a function to acquire the value.
			 * @param name The name of the variable to use in the expression.
			 * @param getterFunction
			 */
			void RegisterVariableViaGetter(const std::string& name, function<double()> getterFunction);

			/**
			 * Deregister a variable.
			 * This is suitable to deregister a variable registered with any RegisterVariable method.
			 * @param name The name of the variable you want to deregister.
			 */
			void DeregisterVariable(const std::string& name);

			/**
			 * Register a variable via a shared pointer.
			 * The variable's type needs to be able to convert to double.
			 * @note The variable needs to be in scope for the lifetime of the
			 * CalculatorWithVariables instance or until DeregisterVariable() is
			 * called with the same name.
			 * @param name The name of the variable to use in an expression.
			 * @param variable The shared pointer to the variable.
			 */
			template< typename T >
			void RegisterVariable(const std::string& name, shared_ptr<T> variable)
			{
				RegisterVariableViaGetter(name,bind(&SharedPtrValueDelegate<T>::GetValue,make_shared< SharedPtrValueDelegate<T> > (variable)));
			}

			/**
			 * Register a variable via a reference.
			 * The variable's type needs to be able to convert to double.
			 * @note The variable needs to be in scope for the lifetime of the
			 * CalculatorWithVariables instance or until DeregisterVariable() is
			 * called with the same name.
			 * @param name The name of the variable to use in an expression.
			 * @param variable The shared pointer to the variable.
			 */
			template< typename T >
			void RegisterVariable(const std::string& name, const T& variable)
			{
				typedef shared_ptr< ReferenceValueDelegate<T> > ReferenceValueDelegatePtr;
				RegisterVariableViaGetter(name,bind(&ReferenceValueDelegate<T>::GetValue, ReferenceValueDelegatePtr(new ReferenceValueDelegate<T>(variable))));
			}
			
			/**
			 * Parse a string expression.
			 * @param expression The expression to parse.
			 * @return If the expression is successfully parsed the result, otherwise the object
			 * returned will be false when converted to a bool.
			 */
			optional<double> Parse(const std::string& expression) const;
			
			/**
			 * Get the function binder.
			 * @return The pointer to the function binder used to evaluate variables.
			 */
			shared_ptr<FunctionBinder> GetFunctionBinder() { return mBinder; }

		private:
			/**
			 * Internal method called to get a value via the function binder.
			 * The symbol used in an expression is used as the function.
			 * @param symbol The name of the variable.
			 * @return the value, or 0 if the value could not be found.
			 */
			double GetValue(const std::string& symbol);
			
			/**
			 * Internal method for calculating the modulus since we can't inline it in our rule definition.
			 */
			inline double Modulus(int x, int y);

			/**
			 * Initialisation of the rules.
			 */
			void Initialise();
		
			shared_ptr<FunctionBinder> mBinder;
			qi::rule<std::string::const_iterator, double(), ascii::space_type> expression, term, factor, variable;
		};

		/**
		 * Evaluates statements into true or false.
		 * Supports && and ||.
		 * Does not support grouping. The result is calculated left to right, each expression
		 * is evaluated and the operator to the left of it is applied to the existing result.
		 * For example:
		 * 
		 *	1==2 && 100+1==1 || 11>=10
		 * 
		 * Results in true since processing left to right yields:
		 * 
		 *			false && false || true
		 * ->		(false && false) || true		*
		 * ->		false || true
		 * ->		true
		 * 
		 * *The grouping here is an illustration of the left to right processing and is not
		 * added then parsed.
		 * 
		 * Example usage:
		 * 
		 *		Scalar mass = 120;
		 *		CalculatorWithVariables parser;
		 *		parser.RegisterVariable("mass",mass);
		 *		optional<double> result = parser("mass => 2 * 3");
		 *		if(result)
		 *		{
		 *			if(*result)
		 *			{
		 *				// Condition was true
		 *			}else
		 *			{
		 *				// Condition was false
		 *			}
		 *		}
		 * @see CalculatorWithVariables as this class parses expressions in the same way.
		 */
		struct ConditionalEvaluator : qi::grammar<std::string::const_iterator, bool(), qi::locals<bool>, ascii::space_type>, public Parser
		{
		public:
			/**
			 * Construct with a custom FunctionBinder.
			 * @param binder A pre-configured function binder.
			 */
			ConditionalEvaluator(shared_ptr<FunctionBinder> binder);

			/**
			 * Default constructor that creates a function binder suitable for parsing.
			 */
			ConditionalEvaluator();

			/**
			 * Register a variable for the expression that uses a function to acquire the value.
			 * @param name The name of the variable to use in the expression.
			 * @param getterFunction
			 */
			void RegisterVariableViaGetter(const std::string& name, function<double()> getterFunction);

			/**
			 * Deregister a variable.
			 * This is suitable to deregister a variable registered with any RegisterVariable method.
			 * @param name The name of the variable you want to deregister.
			 */
			void DeregisterVariable(const std::string& name);

			/**
			 * Register a variable via a shared pointer.
			 * The variable's type needs to be able to convert to double.
			 * @note The variable needs to be in scope for the lifetime of the
			 * CalculatorWithVariables instance or until DeregisterVariable() is
			 * called with the same name.
			 * @param name The name of the variable to use in an expression.
			 * @param variable The shared pointer to the variable.
			 */
			template< typename T >
			void RegisterVariable(const std::string& name, shared_ptr<T> variable)
			{
				RegisterVariableViaGetter(name,bind(&SharedPtrValueDelegate<T>::GetValue,make_shared< SharedPtrValueDelegate<T> > (variable)));
			}

			/**
			 * Register a variable via a reference.
			 * The variable's type needs to be able to convert to double.
			 * @note The variable needs to be in scope for the lifetime of the
			 * CalculatorWithVariables instance or until DeregisterVariable() is
			 * called with the same name.
			 * @param name The name of the variable to use in an expression.
			 * @param variable The shared pointer to the variable.
			 */
			template< typename T >
			void RegisterVariable(const std::string& name, const T& variable)
			{
				typedef shared_ptr< ReferenceValueDelegate<T> > ReferenceValueDelegatePtr;
				RegisterVariableViaGetter(name,bind(&ReferenceValueDelegate<T>::GetValue, ReferenceValueDelegatePtr(new ReferenceValueDelegate<T>(variable))));
			}
			
			/**
			 * Parse a string expression.
			 * @param expression The expression to parse.
			 * @return If the expression is successfully parsed the result, otherwise the object
			 * returned will be false when converted to a bool.
			 */
			optional<bool> Parse(const std::string& expression) const;
			
			/**
			 * Get the function binder.
			 * @return The pointer to the function binder used to evaluate variables.
			 */
			shared_ptr<FunctionBinder> GetFunctionBinder() { return mBinder; }

		private:
			/**
			 * Internal method called to get a value via the function binder.
			 * The symbol used in an expression is used as the function.
			 * @param symbol The name of the variable.
			 * @return 
			 */
			double GetValue(const std::string& symbol);
			
			/**
			 * Internal method for calculating the modulus since we can't inline it in our rule definition.
			 */
			inline double Modulus(int x, int y);

			/**
			 * Initialisation of the rules.
			 */
			void Initialise();
		
			shared_ptr<FunctionBinder> mBinder;
			qi::rule<std::string::const_iterator, double(), ascii::space_type> expression, term, factor, variable;
			qi::rule<std::string::const_iterator, bool(), qi::locals<double>, ascii::space_type> condition;
			qi::rule<std::string::const_iterator, bool(), qi::locals<bool>, ascii::space_type> compoundCondition;
		};
		
		/**
		 * Simple variable substitution parser.
		 * Variables are prefixed with '$' for example, "$MYVARAIABLE"
		 */
		class VariableSubstitutor : public Parser
		{
		public:
			/**
			 * Construct with a custom FunctionBinder.
			 * @param binder A pre-configured function binder.
			 */
			VariableSubstitutor(shared_ptr<FunctionBinder> binder);

			/**
			 * Default constructor that creates a function binder suitable for parsing.
			 */
			VariableSubstitutor();

			VariableSubstitutor(const VariableSubstitutor& other);
			VariableSubstitutor& operator=(const VariableSubstitutor& other);

			/**
			 * Register a variable for the expression that uses a function to acquire the value.
			 * @param name The name of the variable to use in the expression.
			 * @param getterFunction
			 */
			void RegisterVariableViaGetter(const std::string& name, function<std::string()> getterFunction);

			/**
			 * Deregister a variable.
			 * This is suitable to deregister a variable registered with any RegisterVariable method.
			 * @param name The name of the variable you want to deregister.
			 */
			void DeregisterVariable(const std::string& name);

			/**
			 * Register a variable via a shared pointer.
			 * The variable's type needs to be able to convert to double.
			 * @note The variable needs to be in scope for the lifetime of the VariableSubstitutor instance
			 * or until DeregisterVariable() is called with the same name.
			 * @param name The name of the variable to use in an expression.
			 * @param variable The shared pointer to the variable.
			 */
			template< typename T >
			void RegisterVariable(const std::string& name, shared_ptr<T> variable)
			{
				RegisterVariableViaGetter(name,bind(&SharedPtrValueDelegate<T>::GetValue,make_shared< SharedPtrValueDelegate<T> > (variable)));
			}

			/**
			 * Register a variable via a reference.
			 * The variable's type needs to be able to convert to double.
			 * @note The variable needs to be in scope for the lifetime of the VariableSubstitutor instance
			 * or until DeregisterVariable() is called with the same name.
			 * @param name The name of the variable to use in an expression.
			 * @param variable The shared pointer to the variable.
			 */
			template< typename T >
			void RegisterVariable(const std::string& name, const T& variable)
			{
				typedef shared_ptr< ReferenceValueDelegate<T> > ReferenceValueDelegatePtr;
				RegisterVariableViaGetter(name,bind(&ReferenceValueDelegate<T>::GetValue, ReferenceValueDelegatePtr(new ReferenceValueDelegate<T>(variable))));
			}
			
			/**
			 * Parse a string expression.
			 * @param expression The expression to parse.
			 * @return The parsed result.
			 */
			std::string Parse(std::string expression) const;
			
			/**
			 * Get the function binder.
			 * @return The pointer to the function binder used to evaluate variables.
			 */
			shared_ptr<FunctionBinder> GetFunctionBinder() { return mBinder; }
			
		private:
			/**
			 * Internal method called to get a value via the function binder.
			 * The symbol used in an expression is used as the function.
			 * @param symbol The name of the variable.
			 * @return the value, or an empty string if the value could not be found.
			 */
			std::string GetValue(const std::string& symbol) const;

			shared_ptr<FunctionBinder> mBinder;
		};		
	}
}

#endif

