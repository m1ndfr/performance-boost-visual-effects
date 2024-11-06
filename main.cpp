#include <windows.h>
#include <iostream>
#include <string>

bool PowerShell(const std::wstring& script) {
    std::wstring command = L"powershell -WindowStyle Hidden -NoProfile -Command \"" + script + L"\"";

    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.lpVerb = L"runas";           
    sei.lpFile = L"powershell.exe";  
    sei.lpParameters = command.c_str();
    sei.nShow = SW_HIDE;             

    if (!ShellExecuteEx(&sei)) {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_CANCELLED) {
            std::wcout << L"User refused to grant administrator permissions" << std::endl;
        }
        else {
            std::wcout << L"Failed to run PowerShell script as admin. Error : " << dwError << std::endl;
        }
        return false;
    }
    return true;
}

int main() {
    int choice;

    std::wcout << L"\n\nSelect Option:" << std::endl;
    std::wcout << L"1. Boost settings" << std::endl;
    std::wcout << L"2. Restore settings" << std::endl;
    std::wcout << L"choice : ";
    std::wcin >> choice;

    if (choice == 1) {
        // PowerShell script to disable all visual effects
        std::wstring disabler = LR"(
            $regPath = 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Explorer\VisualEffects'
            $keys = Get-ChildItem -Path $regPath
            foreach ($key in $keys) {
                Set-ItemProperty -Path $key.PSPath -Name 'DefaultAppliedSetting' -Value 0
            }
            # Set "Custom" in Performance Options
            Set-ItemProperty -Path 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Explorer\VisualEffects' -Name 'VisualFXSetting' -Value 2
        )";

        if (PowerShell(disabler)) {
            std::wcout << L"All visual effects have been disabled." << std::endl;
        }
        else {
            std::wcout << L"Failed to disable visual effects." << std::endl;
        }
    }
    else if (choice == 2) {
        // PowerShell script to restore default visual effects
        std::wstring restore = LR"(
            $regPath = 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Explorer\VisualEffects'
            $keys = Get-ChildItem -Path $regPath
            foreach ($key in $keys) {
                Set-ItemProperty -Path $key.PSPath -Name 'DefaultAppliedSetting' -Value 1
            }
            # Set "Let Windows choose what's best for my computer" in Performance Options
            Set-ItemProperty -Path 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Explorer\VisualEffects' -Name 'VisualFXSetting' -Value 1
        )";

        if (PowerShell(restore)) {
            std::wcout << L"Visual effects have been restored to default settings." << std::endl;
        }
        else {
            std::wcout << L"Failed to restore visual effects." << std::endl;
        }
    }
    else {
        std::wcout << L"Invalid choice. Please run the program again and enter 1 or 2." << std::endl;
    }

    return 0;
}
