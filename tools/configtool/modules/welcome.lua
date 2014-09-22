--[[============================================================================
@file    welcome.lua

@author  Daniel Zorychta

@brief   Welcome message.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


==============================================================================]]

--==============================================================================
-- EXTERNAL MODULES
--==============================================================================
require("wx")
require("modules/ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
welcome = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function welcome:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.BoxSizer1 = wx.wxBoxSizer(wx.wxHORIZONTAL);
                ui.FlexGridSizer1 = wx.wxFlexGridSizer(3, 1, 0, 0)

                ui.StaticText1 = wx.wxStaticText(this, wx.wxID_ANY, "Welcome to the dnx RTOS Configuration Tool!", wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxALIGN_LEFT)
                ui.StaticText1:SetFont(wx.wxFont(14, wx.wxDEFAULT, wx.wxFONTSTYLE_NORMAL, wx.wxBOLD, false, "", wx.wxFONTENCODING_DEFAULT))
                ui.FlexGridSizer1:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText3 = wx.wxStaticText(this, wx.wxID_ANY, "\nThe tool provides specified features:")
                ui.FlexGridSizer1:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText4 = wx.wxStaticText(this, wx.wxID_ANY, "    - Project configuration")
                ui.FlexGridSizer1:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText5 = wx.wxStaticText(this, wx.wxID_ANY, "      You do not need to edit configuration files by yourself.")
                ui.FlexGridSizer1:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText6 = wx.wxStaticText(this, wx.wxID_ANY, "      Most of configuration options (flags) are handled by this tool.")
                ui.FlexGridSizer1:Add(ui.StaticText6, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText7 = wx.wxStaticText(this, wx.wxID_ANY, "    - Creators")
                ui.FlexGridSizer1:Add(ui.StaticText7, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText8 = wx.wxStaticText(this, wx.wxID_ANY, "      Creators helps you for example adds a new module.")
                ui.FlexGridSizer1:Add(ui.StaticText8, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.BoxSizer1:Add(ui.FlexGridSizer1, 1, bit.bor(wx.wxALL,wx.wxALIGN_TOP,wx.wxALIGN_CENTER_HORIZONTAL), 0)

                this:SetSizer(ui.BoxSizer1)
                this:SetScrollRate(5, 5)
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function welcome:get_window_name()
        return "Welcome"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function welcome:refresh()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @param  None
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function welcome:is_modified()
        return false
end
