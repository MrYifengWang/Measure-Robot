#include "pch.h"
#include "CMyListCtrl.h"

bool CMyListCtrl::OnDisplayCellColor(int nRow, int nCol, COLORREF& textColor, COLORREF& backColor)
{
  
	backColor = RGB(192, 192, 192);
	for (int i = 0; i < m_redCells.size(); i++) {
		if (m_redCells[i].x == nRow && m_redCells[i].y == nCol) {
		//	textColor = RGB(0, 0, 0);
			backColor = RGB(0x8f, 0xbc, 0x8f);
			return true;  // I want to override the color of this cell
		}
	}

	return true;  // Use default color
}

bool CMyListCtrl::SortColumn(int nCol, bool bAscending) {
    return false;
}

bool CMyListCtrl::isCellRed(int nRow, int nCol) {
	for (int i = 0; i < m_redCells.size(); i++) {
		if (m_redCells[i].x == nRow && m_redCells[i].y == nCol) {
			return true; 
		}
	}
	return false;
}

bool CMyListCtrl::OnEditComplete(int nRow, int nCol, CWnd* pEditor, LV_DISPINFO* pLVDI) {
	if (page2_style == 0) {
		return nCol == 5;
	}
	else if(page2_style==1){
		return nCol == 7;
	}
	return false;
}