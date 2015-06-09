<?php
namespace Ebapi\Form\Widget\Fitment\Motors\Select;

class Model extends \Ebapi\Form\Widget\Fitment\Motors\Select\_Abstract
{
  public static function getEntryName()
  {
    return self::prefix . self::getName();
  }

  protected static function getName()
  {
    return 'Model';
  }

  protected function getTitle()
  {
    return 'Model';
  }

  protected function getPreq()
  {
    return array(
      \Ebapi\Form\Widget\Fitment\Motors\Select\Make::getEntryName(),
    );
  }

  protected function isInYearRange()
  {
    return true;
  }

  protected function isYearDep()
  {
    return false;
  }

  public function getFormEntry()
  {
    return $this->getFormTemplate();
  }

  protected function isMultipleSel() {
    return true;
  }
}
