<?php
namespace Oerp\Delivery\Page\Callee\Download_Sent;

abstract class Abstract_fromView extends \Oerp\Delivery\Page\Callee\Abstract_Download
{
  protected function getQueryParam()
  {
    $vparam = $this->FormPage->getViewParam();
    $cri = $vparam['criteria'];

    if($ex_cri = $vparam['cri_url']){
      $ex_cri = \Oerp\Form\Element\SearchPanel::decodeCri($ex_cri);
      $cri = array_merge($cri, $ex_cri);
    }

    return array(
      'oerp:criteria' => $cri,
    );
  }
}