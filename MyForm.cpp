#include "MyForm.h"
#include "API.h"
#include "DataStorage.h"
#include <limits> 

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array<String^>^ args) {

    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

     
    JPOproject::MyForm form;
    Application::Run(% form);

    return 0;
}