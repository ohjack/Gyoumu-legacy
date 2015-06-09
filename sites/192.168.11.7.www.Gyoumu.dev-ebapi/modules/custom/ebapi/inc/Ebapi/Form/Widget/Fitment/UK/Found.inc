<?php
namespace Ebapi\Form\Widget\Fitment\UK;

class Found extends \Ebapi\Form\Widget\Fitment\AbstractFound
{
  protected function getQuery() {
    $pars = array_keys($this->getValue());
    return new \Ebapi\Query\UK\Fitment(
      $this->getCri($pars), $this->getYearRange());
  }

  protected function isShowFitment()
  {
    return $this->getValue()
           && $this->getValue(\Ebapi\Form\Widget\Fitment\UK\Select\Make::getEntryName())
           && $this->getValue(\Ebapi\Form\Widget\Fitment\UK\Select\Model::getEntryName())
        ;
  }

  protected function postProcFitmentList($recs) {
    $send = array();
    foreach ($recs as $val => $text) {
      $send[$val] = preg_replace('/\[\d{4}-\d{4}\]/', '', $text);
    }
    return $send;
  }
}