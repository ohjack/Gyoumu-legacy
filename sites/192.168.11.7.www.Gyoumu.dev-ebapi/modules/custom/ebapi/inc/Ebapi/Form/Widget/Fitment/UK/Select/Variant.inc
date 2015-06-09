<?php
namespace Ebapi\Form\Widget\Fitment\UK\Select;

class Variant extends \Ebapi\Form\Widget\Fitment\UK\Select\_Abstract
{
  public static function getEntryName()
  {
    return self::prefix . self::getName();
  }

  protected static function getName()
  {
    return 'Variant';
  }

  protected function getTitle()
  {
    return 'Variant';
  }

  protected function getPreq()
  {
    return array(
      \Ebapi\Form\Widget\Fitment\UK\Select\Make::getEntryName(),
      \Ebapi\Form\Widget\Fitment\UK\Select\Model::getEntryName(),
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

  protected function getEleAhah($param, $clear = true) {
    $ele = parent::getEleAhah($param, $clear);
    $ele['#attributes']['size'] = 12;
    return $ele;
  }
}