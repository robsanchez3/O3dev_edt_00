/*
#include <gui/common/FrontendApplication.hpp>

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{
}
*/

#include <gui/common/FrontendApplication.hpp>
#include <touchgfx/lcd/LCD16DebugPrinter.hpp>

LCD16DebugPrinter lcd16DebugPrinter; // Global object

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
  : FrontendApplicationBase(m, heap)
{
  lcd16DebugPrinter.setPosition(0, 0, 440, 60);
  lcd16DebugPrinter.setScale(5);
  lcd16DebugPrinter.setColor(0xFF); //black (0x00)
  Application::setDebugPrinter(&lcd16DebugPrinter);
}