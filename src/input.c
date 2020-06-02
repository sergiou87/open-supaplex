/*
* This file is part of the OpenSupaplex distribution (https://github.com/sergiou87/open-supaplex).
* Copyright (c) 2020 Sergio Padrino
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "input.h"

#include "controller.h"
#include "keyboard.h"

uint8_t isUpButtonPressed(void)
{
    return (gIsUpKeyPressed
            || getGameControllerY() < 0);
}

uint8_t isDownButtonPressed(void)
{
    return (gIsDownKeyPressed
            || getGameControllerY() > 0);
}

uint8_t isLeftButtonPressed(void)
{
    return (gIsLeftKeyPressed
            || getGameControllerX() < 0);
}

uint8_t isRightButtonPressed(void)
{
    return (gIsRightKeyPressed
            || getGameControllerX() > 0);
}

uint8_t isActionButtonPressed(void)
{
    return (gIsSpaceKeyPressed
            || getGameControllerButtonX());
}

uint8_t isMenuBackButtonPressed(void)
{
    return (gIsEscapeKeyPressed
            || getGameControllerButtonBack());
}

uint8_t isMenuConfirmButtonPressed(void)
{
    return (gIsEnterPressed || getGameControllerConfirmButton());
}

uint8_t isMenuCancelButtonPressed(void)
{
    return (gIsEscapeKeyPressed
            || getGameControllerCancelButton()
            || getGameControllerButtonBack());
}

uint8_t isExitLevelButtonPressed(void)
{
    return getGameControllerButtonBack();
}

uint8_t isStartButtonPressed(void)
{
    return getGameControllerButtonStart();
}

uint8_t isPauseButtonPressed(void)
{
    return (gIsPKeyPressed
            || gIsEscapeKeyPressed
            || getGameControllerButtonStart());
}

uint8_t isToggleGamePanelButtonPressed(void)
{
    return (gIsEnterPressed
            || getGameControllerButtonY());
}

uint8_t isShowNumberOfRedDisksButtonPressed(void)
{
    return (gIsRightShiftPressed
            || getGameControllerButtonB());
}

uint8_t isIncreaseGameSpeedButtonPressed(void)
{
    return (gIsNumpadPlusPressed
            || getGameControllerButtonRightShoulder());
}

uint8_t isDecreaseGameSpeedButtonPressed(void)
{
    return (gIsNumpadMinusPressed
            || getGameControllerButtonLeftShoulder());
}

uint8_t isRotateLevelSetAscendingButtonPressed(void)
{
    return getGameControllerButtonRightShoulder();
}

uint8_t isRotateLevelSetDescendingButtonPressed(void)
{
    return getGameControllerButtonLeftShoulder();
}
