<?php
namespace Ebapi\Form\Widget\Fitment\Motors;

class Found extends \Ebapi\Form\Widget\Fitment\AbstractFound
{
  protected function getQuery() {
    $pars = array_keys($this->getValue());
    return new \Ebapi\Query\Motors\Fitment(
      $this->getCri($pars), $this->getYearRange());
  }

  protected function isShowFitment()
  {
    return $this->getValue()
           && $this->getValue(\Ebapi\Form\Widget\Fitment\Motors\Select\Make::getEntryName())
           && $this->getValue(\Ebapi\Form\Widget\Fitment\Motors\Select\Model::getEntryName())
        ;
  }
}