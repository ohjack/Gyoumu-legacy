<?php
namespace Ebapi\Form\Widget\Fitment\Motors\Select;

abstract class _Abstract extends \Ebapi\Form\Widget\Fitment\Select\_Abstract {
  protected function getQuery() {
    return new \Ebapi\Query\Motors\Fitment(
      $this->getCri($this->getPreq()),
      $this->isInYearRange() ? $this->getYearRange() : null
    );
  }
}