<?php
namespace Ebapi\Form\Widget\Fitment\UK;

class Year extends \Ebapi\Form\Widget\Fitment\AbstractYear
{
  protected function getQuery() {
    return new \Ebapi\Query\UK\Fitment(
      $this->getCri(array('Make', 'Model', 'Submodel')), null);
  }

  protected function getOpts() {
    $opts = parent::getOpts();
    $opts_send = array();

    foreach ($opts as $key => $opt) {
      if ($key == '0') {
        $opts_send[$key] = $opt;
      }
      else {
        $yrs = explode('|', $opt);

        foreach ($yrs as $yr) {
          if (!isset($opts_send[$yr])) {
            $opts_send[$yr] = $yr;
          }
        }
      }
    }
    ksort($opts_send);
    return $opts_send;
  }
}

