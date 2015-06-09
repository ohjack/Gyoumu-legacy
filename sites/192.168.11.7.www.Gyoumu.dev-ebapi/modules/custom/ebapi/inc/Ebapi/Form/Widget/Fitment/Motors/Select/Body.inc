<?php
namespace Ebapi\Form\Widget\Fitment\Motors\Select;

class Body extends \Ebapi\Form\Widget\Fitment\Motors\Select\_Abstract
{
  public static function getEntryName()
  {
    return self::prefix . self::getName();
  }

  protected static function getName()
  {
    return 'Body';
  }

  protected function getTitle()
  {
    return 'Body';
  }

  protected function getPreq()
  {
    return array(
      \Ebapi\Form\Widget\Fitment\Motors\Select\Make::getEntryName(),
      \Ebapi\Form\Widget\Fitment\Motors\Select\Model::getEntryName(),
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