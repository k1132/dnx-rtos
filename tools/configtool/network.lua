--[[============================================================================
@file    network.lua

@author  Daniel Zorychta

@brief   Network configuration script.

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
require("ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
network = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}
ID.BUTTON_SAVE = wx.wxNewId()
ID.CHECKBOX_ENABLE = wx.wxNewId()
ID.COMBOBOX_IF_PATH = wx.wxNewId()
ID.TEXTCTRL_MAC0 = wx.wxNewId()
ID.TEXTCTRL_MAC1 = wx.wxNewId()
ID.TEXTCTRL_MAC2 = wx.wxNewId()
ID.TEXTCTRL_MAC3 = wx.wxNewId()
ID.TEXTCTRL_MAC4 = wx.wxNewId()
ID.TEXTCTRL_MAC5 = wx.wxNewId()


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function set state (enable/disable) of specified controls
-- @param  state        state to set (bool)
-- @return None
--------------------------------------------------------------------------------
local function set_controls_state(state)
        ui.TextCtrl_MAC0:Enable(state)
        ui.TextCtrl_MAC1:Enable(state)
        ui.TextCtrl_MAC2:Enable(state)
        ui.TextCtrl_MAC3:Enable(state)
        ui.TextCtrl_MAC4:Enable(state)
        ui.TextCtrl_MAC5:Enable(state)
        ui.ComboBox_if_path:Enable(state)
end


--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()
        ui.CheckBox_enable:SetValue(ct:get_module_state("NETWORK"))
        ui.TextCtrl_MAC0:SetValue(ct:key_read(config.project.key.NETWORK_MAC_ADDR_0):gsub("0x", ""))
        ui.TextCtrl_MAC1:SetValue(ct:key_read(config.project.key.NETWORK_MAC_ADDR_1):gsub("0x", ""))
        ui.TextCtrl_MAC2:SetValue(ct:key_read(config.project.key.NETWORK_MAC_ADDR_2):gsub("0x", ""))
        ui.TextCtrl_MAC3:SetValue(ct:key_read(config.project.key.NETWORK_MAC_ADDR_3):gsub("0x", ""))
        ui.TextCtrl_MAC4:SetValue(ct:key_read(config.project.key.NETWORK_MAC_ADDR_4):gsub("0x", ""))
        ui.TextCtrl_MAC5:SetValue(ct:key_read(config.project.key.NETWORK_MAC_ADDR_5):gsub("0x", ""))
        ui.ComboBox_if_path:SetValue(ct:key_read(config.project.key.NETWORK_ETHIF_FILE):gsub('"', ''))
        set_controls_state(ui.CheckBox_enable:GetValue())
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function on_button_save_click()
        ct:enable_module("NETWORK", ui.CheckBox_enable:GetValue())
        ct:key_write(config.project.key.NETWORK_MAC_ADDR_0, "0x"..ui.TextCtrl_MAC0:GetValue())
        ct:key_write(config.project.key.NETWORK_MAC_ADDR_1, "0x"..ui.TextCtrl_MAC1:GetValue())
        ct:key_write(config.project.key.NETWORK_MAC_ADDR_2, "0x"..ui.TextCtrl_MAC2:GetValue())
        ct:key_write(config.project.key.NETWORK_MAC_ADDR_3, "0x"..ui.TextCtrl_MAC3:GetValue())
        ct:key_write(config.project.key.NETWORK_MAC_ADDR_4, "0x"..ui.TextCtrl_MAC4:GetValue())
        ct:key_write(config.project.key.NETWORK_MAC_ADDR_5, "0x"..ui.TextCtrl_MAC5:GetValue())
        ct:key_write(config.project.key.NETWORK_ETHIF_FILE, '"'..ui.ComboBox_if_path:GetValue()..'"')

        ui.Button_save:Enable(false)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when module enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function checkbox_enable_clicked(this)
        set_controls_state(this:IsChecked())
        ui.Button_save:Enable(true)
end


--------------------------------------------------------------------------------
-- @brief  Function handle TextCtrl object to check that text is a hex number
-- @param  this         TextCtrl object
-- @return None
--------------------------------------------------------------------------------
local function textctrl_only_hex(this)
        -- function check if char is a hex value
        local function ischarhex(char)
                char:upper()
                return (char >= '0' and char <= '9') or (char >= 'A' and char <= 'F')
        end

        -- analyze of TextCtrl value
        if this:IsModified() then
                local text  = this:GetValue():upper()
                local char1 = string.char(text:byte(1))
                local char2 = string.char(text:byte(2))
                text        = nil

                if ischarhex(char1) then text = char1 end
                if ischarhex(char2) then text = char1..char2 end

                if text then
                        this:SetValue(text)
                        this:SetInsertionPointEnd()
                else
                        this:Clear()
                end

                ui.Button_save:Enable(true)
        end
end


--------------------------------------------------------------------------------
-- @brief  Events are called when value of particular TextCtrl of MAC address is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function mac0_textctrl_changed(this) textctrl_only_hex(ui.TextCtrl_MAC0) end
local function mac1_textctrl_changed(this) textctrl_only_hex(ui.TextCtrl_MAC1) end
local function mac2_textctrl_changed(this) textctrl_only_hex(ui.TextCtrl_MAC2) end
local function mac3_textctrl_changed(this) textctrl_only_hex(ui.TextCtrl_MAC3) end
local function mac4_textctrl_changed(this) textctrl_only_hex(ui.TextCtrl_MAC4) end
local function mac5_textctrl_changed(this) textctrl_only_hex(ui.TextCtrl_MAC5) end


--------------------------------------------------------------------------------
-- @brief  Event is called when combobox value is changed
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function combobox_changed()
        ui.Button_save:Enable(true)
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function network:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
                ui.CheckBox_enable = wx.wxCheckBox(this, ID.CHECKBOX_ENABLE, "Enable", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1), 0, wx.wxDefaultValidator, "ID.CHECKBOX_ENABLE")
                ui.FlexGridSizer1:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "MAC address")
                ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 11, 0, 0)
                ui.TextCtrl_MAC0 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC0, "", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_MAC0")
                ui.TextCtrl_MAC0:SetMaxLength(2)
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC0, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText1 = wx.wxStaticText(this, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT1")
                ui.FlexGridSizer2:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.TextCtrl_MAC1 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC1, "", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_MAC1")
                ui.TextCtrl_MAC1:SetMaxLength(2)
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText2 = wx.wxStaticText(this, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT2")
                ui.FlexGridSizer2:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.TextCtrl_MAC2 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC2, "", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_MAC2")
                ui.TextCtrl_MAC2:SetMaxLength(2)
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText3 = wx.wxStaticText(this, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT3")
                ui.FlexGridSizer2:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.TextCtrl_MAC3 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC3, "", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_MAC3")
                ui.TextCtrl_MAC3:SetMaxLength(2)
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText4 = wx.wxStaticText(this, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT4")
                ui.FlexGridSizer2:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.TextCtrl_MAC4 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC4, "", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_MAC4")
                ui.TextCtrl_MAC4:SetMaxLength(2)
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText5 = wx.wxStaticText(this, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT5")
                ui.FlexGridSizer2:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.TextCtrl_MAC5 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC5, "", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_MAC5")
                ui.TextCtrl_MAC5:SetMaxLength(2)
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC5, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticBoxSizer1:Add(ui.FlexGridSizer2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Ethernet device path")
                ui.ComboBox_if_path = wx.wxComboBox(this, ID.COMBOBOX_IF_PATH, "", wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.COMBOBOX_IF_PATH")
                ui.ComboBox_if_path:SetToolTip("This is a path to the Ethernet device file.")
                ui.ComboBox_if_path:Append("/dev/eth0")
                ui.StaticBoxSizer2:Add(ui.ComboBox_if_path, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                ui.StaticLine2 = wx.wxStaticLine(this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL)
                ui.FlexGridSizer1:Add(ui.StaticLine2, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 0)

                ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "&Save", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL, wx.wxALIGN_RIGHT, wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                this:SetSizer(ui.FlexGridSizer1)
                this:SetScrollRate(5, 5)

                --
                this:Connect(ID.CHECKBOX_ENABLE,  wx.wxEVT_COMMAND_CHECKBOX_CLICKED,  checkbox_enable_clicked)
                this:Connect(ID.TEXTCTRL_MAC0,    wx.wxEVT_COMMAND_TEXT_UPDATED,      mac0_textctrl_changed  )
                this:Connect(ID.TEXTCTRL_MAC1,    wx.wxEVT_COMMAND_TEXT_UPDATED,      mac1_textctrl_changed  )
                this:Connect(ID.TEXTCTRL_MAC2,    wx.wxEVT_COMMAND_TEXT_UPDATED,      mac2_textctrl_changed  )
                this:Connect(ID.TEXTCTRL_MAC3,    wx.wxEVT_COMMAND_TEXT_UPDATED,      mac3_textctrl_changed  )
                this:Connect(ID.TEXTCTRL_MAC4,    wx.wxEVT_COMMAND_TEXT_UPDATED,      mac4_textctrl_changed  )
                this:Connect(ID.TEXTCTRL_MAC5,    wx.wxEVT_COMMAND_TEXT_UPDATED,      mac5_textctrl_changed  )
                this:Connect(ID.COMBOBOX_IF_PATH, wx.wxEVT_COMMAND_COMBOBOX_SELECTED, combobox_changed       )
                this:Connect(ID.COMBOBOX_IF_PATH, wx.wxEVT_COMMAND_TEXT_UPDATED,      combobox_changed       )
                this:Connect(ID.BUTTON_SAVE,      wx.wxEVT_COMMAND_BUTTON_CLICKED,    on_button_save_click   )
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function network:get_window_name()
        return "Network"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function network:refresh()
        load_controls()
        ui.Button_save:Enable(false)
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @param  None
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function network:is_modified()
        return ui.Button_save:IsEnabled()
end
