#include "formula.h"

// #include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

// Реализуйте следующие методы:
Formula::Formula(std::string expression) 
	: ast_(ParseFormulaAST(std::move(expression)))
{
	auto cells = ast_.GetCells();
	cells.unique();
	cells.sort();
	cells_ = {cells.begin(), cells.end()};
}
    
FormulaInterface::Value Formula::Evaluate(const SheetInterface& arg) const {
	try {
		double res = ast_.Execute(arg);
		return res;
	}
	catch(const FormulaError& fe) {
		return fe;
	}
}
    
std::string Formula::GetExpression() const {
	std::ostringstream oss;
	ast_.PrintFormula(oss);
	return oss.str();
}

std::vector<Position> Formula::GetReferencedCells() const {
	return cells_;
}

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch(...) {
        throw FormulaException{"Unable to parse: "s.append(expression)};
    }
}
