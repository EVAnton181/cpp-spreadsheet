#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

std::vector<Position> Cell::Impl::GetReferencedCells() const {
  return {};
}

Cell::Value Cell::EmptyImpl::GetValue([[maybe_unused]] SheetInterface& sheet) const {
	return ""s;
}

std::string Cell::EmptyImpl::GetString() const {
	return ""s;
}

Cell::TextImpl::TextImpl(std::string text)
  : data_(std::move(text)) 
{
}
		
Cell::Value Cell::TextImpl::GetValue([[maybe_unused]] SheetInterface& sheet) const {
	return data_; 
}
std::string Cell::TextImpl::GetString() const {
  	return data_; 
}


Cell::FormulaImpl::FormulaImpl(std::string formula)
	: data_(std::move(ParseFormula(std::move(formula))))
{
}
	
Cell::Value Cell::FormulaImpl::GetValue(SheetInterface& sheet) const {
	  return std::visit(FormulaVisitor(), data_->Evaluate(sheet));
}

std::string Cell::FormulaImpl::GetString() const {
  return "="s.append(data_->GetExpression());
}

std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const {
  return data_->GetReferencedCells();
}
	
// Реализуйте следующие методы
// Реализуйте следующие методы
Cell::Cell(Sheet& sheet) 
    : impl_(std::make_unique<Cell::EmptyImpl>(EmptyImpl{})) 
    , sheet_(sheet) 
{
}
    
Cell::~Cell() = default;

bool Cell::IsModified() const {
    return cache_;
}

void Cell::SetCache(Value&& val) const {
    cache_.value_ = std::forward<Value>(val);
    cache_.modify_ = false;
}

void Cell::Set(std::string text) {
    if(text.empty()) {
        impl_ = std::make_unique<EmptyImpl>();
    }
    else if(text.front() == '=' && text.size() > 1u) {
        try {
            impl_ = std::make_unique<FormulaImpl>(FormulaImpl{text.substr(1u)});
            for(const auto& ref_cell : impl_->GetReferencedCells()) {
                auto cell_ptr = sheet_.GetCell(ref_cell);
                if(!cell_ptr) {
                    sheet_.SetCell(ref_cell, ""s);
                }
            }
        }
        catch(...) {
            throw FormulaException{"Unable to parse: "s.append(text)};
        }
    }
    else {
        impl_ = std::make_unique<TextImpl>(TextImpl{text});
    }
    cache_.modify_ = true;
}


void Cell::Clear() {
    impl_.reset(nullptr);
	cache_.modify_ = true;
}
struct ValueVisitor {
    Cell::Value operator() (std::string str) const {
        return str.front() == '\'' ? str.substr(1u) : str;
    }
    Cell::Value operator() (double d) const {
        return d;
    }
    Cell::Value operator() (FormulaError fe) const {
        return fe;
    }
};

Cell::Value Cell::GetValue() const {
    const auto& dependency = GetReferencedCells();
    if(!cache_ || std::any_of(dependency.begin(), dependency.end(), 
        [this] (const auto& pos) {
                                     const Cell* cell_ptr_ = reinterpret_cast<const Cell*>(sheet_.GetCell(pos));
                                     return cell_ptr_->IsModified();
                                 })) {
        
        auto value = impl_ ? std::visit(ValueVisitor(), impl_->GetValue(sheet_)) : Cell::Value{};
        SetCache(std::move(value));
    }
    return cache_;
}

std::string Cell::GetText() const {
    if(impl_) {
        return impl_->GetString();
    }
    return {};
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

Cell::CellCache::operator bool() const {
    return !modify_;
}

Cell::CellCache::operator Value() const {
    return value_;
}

