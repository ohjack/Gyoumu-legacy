<?php
namespace Ebapi\Form\Widget\Fitment\Select;

class Make extends _Abstract
{
  public static function getEntryName()
  {
    return self::prefix . self::getName();
  }

  protected static function getName()
  {
    return 'Make';
  }

  protected function getTitle()
  {
    return 'Make';
  }

  protected function getPreq()
  {
    return array();
  }

  protected function isInYearRange()
  {
    return false;
  }

  protected function isYearDep()
  {
    return false;
  }

  public function getFormEntry()
  {
    return $this->getFormTemplate();
  }
}
