#include "EpubReaderFootnotesActivity.h"

#include <GfxRenderer.h>
#include <I18n.h>

#include "MappedInputManager.h"
#include "components/UITheme.h"
#include "fontIds.h"

int EpubReaderFootnotesActivity::getPageItems() const {
  return UITheme::getNumberOfItemsPerPage(renderer, true, false, true, false);
}

void EpubReaderFootnotesActivity::onEnter() {
  Activity::onEnter();
  selectedIndex = 0;
  requestUpdate();
}

void EpubReaderFootnotesActivity::onExit() { Activity::onExit(); }

void EpubReaderFootnotesActivity::loop() {
  const int pageItems = getPageItems();
  const int totalItems = static_cast<int>(footnotes.size());

  if (mappedInput.wasReleased(MappedInputManager::Button::Confirm)) {
    if (!footnotes.empty() && selectedIndex >= 0 && selectedIndex < static_cast<int>(footnotes.size())) {
      setResult(FootnoteResult{footnotes[selectedIndex].href});
      finish();
    }
  } else if (mappedInput.wasReleased(MappedInputManager::Button::Back)) {
    ActivityResult result;
    result.isCancelled = true;
    setResult(std::move(result));
    finish();
  }

  buttonNavigator.onNextRelease([this, totalItems] {
    selectedIndex = ButtonNavigator::nextIndex(selectedIndex, totalItems);
    requestUpdate();
  });

  buttonNavigator.onPreviousRelease([this, totalItems] {
    selectedIndex = ButtonNavigator::previousIndex(selectedIndex, totalItems);
    requestUpdate();
  });

  buttonNavigator.onNextContinuous([this, totalItems, pageItems] {
    selectedIndex = ButtonNavigator::nextPageIndex(selectedIndex, totalItems, pageItems);
    requestUpdate();
  });

  buttonNavigator.onPreviousContinuous([this, totalItems, pageItems] {
    selectedIndex = ButtonNavigator::previousPageIndex(selectedIndex, totalItems, pageItems);
    requestUpdate();
  });
}

void EpubReaderFootnotesActivity::render(RenderLock&&) {
  renderer.clearScreen();
  const auto& metrics = UITheme::getInstance().getMetrics();
  const Rect content = UITheme::getContentRect(renderer, true, false, true);

  const int headerY = content.y - metrics.headerHeight - metrics.verticalSpacing;
  GUI.drawHeader(renderer, Rect{content.x, headerY, content.width, metrics.headerHeight}, tr(STR_FOOTNOTES));

  const int totalItems = static_cast<int>(footnotes.size());

  if (totalItems == 0) {
    const int emptyX = content.x + (content.width - renderer.getTextWidth(UI_10_FONT_ID, tr(STR_NO_FOOTNOTES))) / 2;
    renderer.drawText(UI_10_FONT_ID, emptyX, content.y + content.height / 2, tr(STR_NO_FOOTNOTES));
  } else {
    GUI.drawList(renderer, content, totalItems, selectedIndex, [this](int i) -> std::string {
      std::string label = footnotes[i].number;
      if (label.empty()) {
        label = tr(STR_LINK);
      }
      return label;
    });
  }

  const auto labels = mappedInput.mapLabels(tr(STR_BACK), tr(STR_SELECT), tr(STR_DIR_UP), tr(STR_DIR_DOWN));
  GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);
  renderer.displayBuffer();
}
