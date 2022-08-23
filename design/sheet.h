#pragma once

#include "cell.h"
#include "common.h"

#include <functional>

class Sheet : public SheetInterface {
public:
	~Sheet();

	void SetCell(Position pos, std::string text) override;

	const CellInterface* GetCell(Position pos) const override;
	CellInterface* GetCell(Position pos) override;

	void ClearCell(Position pos) override;

	Size GetPrintableSize() const override;

	void PrintValues(std::ostream& output) const override;
	void PrintTexts(std::ostream& output) const override;

private:
	// хэшер для data_
	struct position_hash { 
		size_t operator()(const Position& p) const;
	};
	
	std::unordered_map<Position, std::unique_ptr<CellInterface>, position_hash> data_;
	
	// Возвращает координаты левого верхнего и правого нижнего углов
	std::pair<Position, Position> Sheet::GetLeftRightCorners() const;
	
	// Проверяет на циклическую зависимость ячеек
	bool CheckCircularDependency(const CellInterface* cell, Position this_pos) const;
};
