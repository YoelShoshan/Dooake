#include "stdafx.h"
#include "guiItem.h"

IGuiItem::IGuiItem()
{
	m_pParentScreen = NULL;

	m_bActive = true;
	m_bRender = true;
	m_bFocus = false;
}

void IGuiItem::SetParent(CMenuScreen* pParent)
{
	m_pParentScreen = pParent;
}

void IGuiItem::SetFocus(void)
{
	if (!m_pParentScreen)
		return;

	m_pParentScreen->UnFocusAll();

	m_bFocus = true;

}