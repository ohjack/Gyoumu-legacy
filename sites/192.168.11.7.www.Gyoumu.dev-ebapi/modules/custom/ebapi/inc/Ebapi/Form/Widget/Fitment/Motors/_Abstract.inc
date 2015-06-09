<?php
namespace Ebapi\Form\Widget\Fitment\Motors;

abstract class _Abstract extends \Ebapi\Form\Widget\Fitment\_Abstract {
  function getQuery() {
    return new \Ebapi\Query\Motors\Fitment(
      $this->getCri($this->getPreq()),
      $this->isInYearRange() ? $this->getYearRange() : null
    );
  }
}