#include <gui/selector_screen/SelectorView.hpp>
#include <gui/selector_screen/SelectorPresenter.hpp>

#include <stdio.h>




SelectorPresenter::SelectorPresenter(SelectorView& v)
    : view(v)
{

}

void SelectorPresenter::activate()
{
	view.initTherapyContext(model->getTherapyCtx());
	view.initSelectionContext(0);
}

void SelectorPresenter::deactivate()
{

}

void SelectorPresenter::EndSelection(void)
{
	model->EndSelection();
}

void SelectorPresenter::StartGeneration(void)
{
	model->StartGeneration();
}

void SelectorPresenter::okClicked(uint8_t targetValueID, uint16_t value)
{
	printf("OK button clicked, selection ID: %d, value: %d\n", targetValueID, value);
	model->setTherapyTargetValue(targetValueID, value);
}

void SelectorPresenter::cancelClicked()
{
	model->CancelSelection();
}

void SelectorPresenter::userCancelled()
{
	model->userCancelled();
}

void SelectorPresenter::setTherapyTargetValue(uint8_t targetValueID, uint16_t value)
{
	model->setTherapyTargetValue(targetValueID, value);
}

uint32_t SelectorPresenter::getConfiguredTime()
{
	return model->getConfiguredTime();
}

uint32_t SelectorPresenter::getConfiguredVolume()
{
	return model->getConfiguredVolume();
}

void SelectorPresenter::onSelectionAction(uint8_t selectionStep)
{
	model->onSelectionAction(selectionStep);
}

void SelectorPresenter::onSliderAction(uint8_t step, uint8_t targetValueID, uint16_t value)
{
	model->setTherapyTargetValue(targetValueID, value);
	model->onSliderAction(step);
}

