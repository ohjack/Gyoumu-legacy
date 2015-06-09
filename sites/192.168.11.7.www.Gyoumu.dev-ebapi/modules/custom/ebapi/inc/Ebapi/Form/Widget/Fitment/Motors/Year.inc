<?php
namespace Ebapi\Form\Widget\Fitment\Motors;

class Year extends \Ebapi\Form\Widget\Fitment\AbstractYear
{
  protected function getQuery() {
    return new \Ebapi\Query\Motors\Fitment(
      $this->getCri(array('Make', 'Model', 'Submodel')), null);
  }
}

