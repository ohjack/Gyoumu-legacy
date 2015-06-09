<?php
namespace Ebapi\Form\Widget\Fitment\Select;

class Submodel extends _Abstract
{
  public static function getEntryName()
  {
    return self::prefix . self::getName();
  }

  protected static function getName()
  {
    return 'Submodel';
  }

  protected function getTitle()
  {
    return 'Submodel';
  }

  protected function getPreq()
  {
    return array(
      \Ebapi\Form\Widget\Fitment\Select\Make::getEntryName(),
      \Ebapi\Form\Widget\Fitment\Select\Model::getEntryName(),
    );
  }

  protected function isInYearRange()
  {
    return false;
  }

  protected function isYearDep()
  {
    return true;
  }

  public function getFormEntry()
  {
    return $this->getFormTemplate();
  }
}