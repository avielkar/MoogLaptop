/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        checkbox.cpp
// Purpose:     Part of the widgets sample showing wxCheckBox
// Author:      Dimitri Schoolwerth, Vadim Zeitlin
// Created:     27 Sep 2003
// Id:          $Id: checkbox.cpp,v 1.10 2005/08/28 08:54:54 MBN Exp $
// Copyright:   (c) 2003 wxWindows team
// License:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"

    #include "wx/sizer.h"
#endif

#include "widgets.h"

#include "icons/checkbox.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    CheckboxPage_Reset = 100,
    CheckboxPage_ChangeLabel,
    CheckboxPage_Check,
    CheckboxPage_Uncheck,
    CheckboxPage_PartCheck,
    CheckboxPage_ChkRight,
    CheckboxPage_Checkbox
};

enum
{
    CheckboxKind_2State,
    CheckboxKind_3State,
    CheckboxKind_3StateUser
};

// ----------------------------------------------------------------------------
// CheckBoxWidgetsPage
// ----------------------------------------------------------------------------

class CheckBoxWidgetsPage : public WidgetsPage
{
public:
    CheckBoxWidgetsPage(wxBookCtrlBase *book, wxImageList *imaglist);
    virtual ~CheckBoxWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_checkbox; }

protected:
    // event handlers
    void OnCheckBox(wxCommandEvent& event);

    void OnStyleChange(wxCommandEvent& event);
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonChangeLabel(wxCommandEvent& event);

    void OnButtonCheck(wxCommandEvent&) { m_checkbox->SetValue(true); }
    void OnButtonUncheck(wxCommandEvent&) { m_checkbox->SetValue(false); }
    void OnButtonPartCheck(wxCommandEvent&)
    {
        m_checkbox->Set3StateValue(wxCHK_UNDETERMINED);
    }

    void Is3State(wxUpdateUIEvent& event)
    {
        event.Enable( m_checkbox->Is3State() );
    }


    // reset the wxCheckBox parameters
    void Reset();

    // (re)create the wxCheckBox
    void CreateCheckbox();

    // the controls
    // ------------

    // the contols to choose the checkbox style
    wxCheckBox *m_chkRight;
    wxRadioBox *m_radioKind;

    // the checkbox itself and the sizer it is in
    wxCheckBox *m_checkbox;
    wxSizer *m_sizerCheckbox;

    // the text entries for command parameters
    wxTextCtrl *m_textLabel;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(CheckBoxWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CheckBoxWidgetsPage, WidgetsPage)
    EVT_CHECKBOX(CheckboxPage_Checkbox, CheckBoxWidgetsPage::OnCheckBox)

    EVT_BUTTON(CheckboxPage_Reset, CheckBoxWidgetsPage::OnButtonReset)
    EVT_BUTTON(CheckboxPage_ChangeLabel, CheckBoxWidgetsPage::OnButtonChangeLabel)
    EVT_BUTTON(CheckboxPage_Check, CheckBoxWidgetsPage::OnButtonCheck)
    EVT_BUTTON(CheckboxPage_Uncheck, CheckBoxWidgetsPage::OnButtonUncheck)
    EVT_BUTTON(CheckboxPage_PartCheck, CheckBoxWidgetsPage::OnButtonPartCheck)

    EVT_UPDATE_UI(CheckboxPage_PartCheck, CheckBoxWidgetsPage::Is3State)

    EVT_RADIOBOX(wxID_ANY, CheckBoxWidgetsPage::OnStyleChange)
    EVT_CHECKBOX(CheckboxPage_ChkRight, CheckBoxWidgetsPage::OnStyleChange)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(CheckBoxWidgetsPage, wxT("CheckBox"));

CheckBoxWidgetsPage::CheckBoxWidgetsPage(wxBookCtrlBase *book,
                                         wxImageList *imaglist)
                  : WidgetsPage(book)
{
    imaglist->Add(wxBitmap(checkbox_xpm));

    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, _T("&Set style"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkRight = CreateCheckBoxAndAddToSizer
                 (
                    sizerLeft,
                    _T("&Right aligned"),
                    CheckboxPage_ChkRight
                 );

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    static const wxString kinds[] =
    {
        _T("usual &2-state checkbox"),
        _T("&3rd state settable by program"),
        _T("&user-settable 3rd state"),
    };

    m_radioKind = new wxRadioBox(this, wxID_ANY, _T("&Kind"),
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(kinds), kinds,
                                 1);
    sizerLeft->Add(m_radioKind, 0, wxGROW | wxALL, 5);
    wxButton *btn = new wxButton(this, CheckboxPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, _T("&Operations"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    sizerMiddle->Add(CreateSizerWithTextAndButton(CheckboxPage_ChangeLabel,
                                                     _T("Change label"),
                                                     wxID_ANY,
                                                     &m_textLabel),
                     0, wxALL | wxGROW, 5);
    sizerMiddle->Add(new wxButton(this, CheckboxPage_Check, _T("&Check it")),
                     0, wxALL | wxGROW, 5);
    sizerMiddle->Add(new wxButton(this, CheckboxPage_Uncheck, _T("&Uncheck it")),
                     0, wxALL | wxGROW, 5);
    sizerMiddle->Add(new wxButton(this, CheckboxPage_PartCheck,
                                  _T("Put in &3rd state")),
                     0, wxALL | wxGROW, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);
    m_checkbox = new wxCheckBox(this, CheckboxPage_Checkbox, _T("&Check me!"));
    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->Add(m_checkbox, 1, wxCENTRE);
    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->SetMinSize(150, 0);
    m_sizerCheckbox = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 1, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);

    sizerTop->Fit(this);
}

void CheckBoxWidgetsPage::Reset()
{
    m_chkRight->SetValue(false);
    m_radioKind->SetSelection(CheckboxKind_2State);
}

void CheckBoxWidgetsPage::CreateCheckbox()
{
    wxString label = m_checkbox->GetLabel();

    size_t count = m_sizerCheckbox->GetChildren().GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_sizerCheckbox->Remove(0);
    }

    delete m_checkbox;

    int flags = 0;
    if ( m_chkRight->IsChecked() )
        flags |= wxALIGN_RIGHT;

    switch ( m_radioKind->GetSelection() )
    {
        default:
            wxFAIL_MSG(_T("unexpected radiobox selection"));
            // fall through

        case CheckboxKind_2State:
            flags |= wxCHK_2STATE;
            break;

        case CheckboxKind_3StateUser:
            flags |= wxCHK_ALLOW_3RD_STATE_FOR_USER;
            // fall through

        case CheckboxKind_3State:
            flags |= wxCHK_3STATE;
            break;
    }

    m_checkbox = new wxCheckBox(this, CheckboxPage_Checkbox, label,
                              wxDefaultPosition, wxDefaultSize,
                              flags);

    m_sizerCheckbox->Add(0, 0, 1, wxCENTRE);
    m_sizerCheckbox->Add(m_checkbox, 1, wxCENTRE);
    m_sizerCheckbox->Add(0, 0, 1, wxCENTRE);
    m_sizerCheckbox->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void CheckBoxWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateCheckbox();
}

void CheckBoxWidgetsPage::OnStyleChange(wxCommandEvent& WXUNUSED(event))
{
    CreateCheckbox();
}

void CheckBoxWidgetsPage::OnButtonChangeLabel(wxCommandEvent& WXUNUSED(event))
{
    m_checkbox->SetLabel(m_textLabel->GetValue());
}

void CheckBoxWidgetsPage::OnCheckBox(wxCommandEvent& event)
{
    wxLogMessage(_T("Test checkbox %schecked (value = %d)."),
                 event.IsChecked() ? _T("") : _T("un"),
                 (int)m_checkbox->Get3StateValue());
}

