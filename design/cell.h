#pragma once

#include "common.h"
#include "formula.h"

using namespace std::literals;


class Cell : public CellInterface {
private:
	struct CellCashe {
	  Value value_;
	  bool modify_ = true;
	};

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
	
	CellCashe cashe_; // кэш значения ячейки если она формула или текст
};

// Базовый класс ячейки
class Impl {
public:
	virtual ~Impl() = default;
	
	virtual Value GetValue(SheetInterface& sheet) const = 0;
	virtual std::string GetString() const = 0;
	virtual std::vector<Position> GetReferencedCells() const;
};

// Класс пустой ячейки
class EmptyImpl final : public Impl {
public:
	EmptyImpl() = default;
	
	Value GetValue(SheetInterface& sheet) const override;    
	std::string GetString() const override;
};

// Класс ячейки с текстом
class TextImpl final : public Impl {
public:
	explicit TextImpl(std::string text);
		
	Value GetValue(SheetInterface& sheet) const override;    
	std::string GetString() const override;
private:
	const std::string data_;
};

// Класс ячейки с формулой
class FormulaImpl final : public Impl {
public:
	explicit FormulaImpl(std::string formula);
	
	Value GetValue(SheetInterface& sheet) const override;
	std::string GetString() const override;
	std::vector<Position> GetReferencedCells() const override;
private:
	std::unique_ptr<FormulaInterface> data_;
};
