#pragma once

#include "common.h"
#include "formula.h"
#include "sheet.h"

using namespace std::literals;



class Sheet;

class Cell : public CellInterface {
private:
	struct CellCache {
	  Value value_;
	  bool modify_ = true;
	  
	  operator bool() const;
	  operator Value() const;
	};
	
	bool IsModified() const;
    void SetCache(Value&& val) const;

public:
	Cell(Sheet& sheet);
	~Cell();

	void Set(std::string text);
	void Clear();

	Value GetValue() const override;
	std::string GetText() const override;

	std::vector<Position> GetReferencedCells() const override;
private:
	class Impl;
	class EmptyImpl;
	class TextImpl;
	class FormulaImpl;
	std::unique_ptr<Impl> impl_;
	
	Sheet& sheet_; // ссылка на лист из которого ячейка
	
	mutable CellCache cache_;                                  // кэш значения ячейки если она формула или текст
};

// Базовый класс ячейки
class Cell::Impl {
public:
	virtual ~Impl() = default;
	
	virtual Cell::Value GetValue(SheetInterface& sheet) const = 0;
	virtual std::string GetString() const = 0;
	virtual std::vector<Position> GetReferencedCells() const;
};

// Класс пустой ячейки
class Cell::EmptyImpl final : public Cell::Impl {
public:
	EmptyImpl() = default;
	
	Cell::Value GetValue([[maybe_unused]] SheetInterface& sheet) const override;    
	std::string GetString() const override;
};

// Класс ячейки с текстом
class Cell::TextImpl final : public Cell::Impl {
public:
	explicit TextImpl(std::string text);
		
	Cell::Value GetValue([[maybe_unused]] SheetInterface& sheet) const override;    
	std::string GetString() const override;
private:
	const std::string data_;
};

// Класс ячейки с формулой
class Cell::FormulaImpl final : public Cell::Impl {
private:
    struct FormulaVisitor {
        Cell::Value operator() (double d) {
            return Cell::Value{d};
        }
        Cell::Value operator() (FormulaError fe) {
            return Cell::Value{fe};
        }
    };
public:
	explicit FormulaImpl(std::string formula);
	
	Cell::Value GetValue(SheetInterface& sheet) const override;
	std::string GetString() const override;
	std::vector<Position> GetReferencedCells() const override;
private:
	std::unique_ptr<FormulaInterface> data_;
};
