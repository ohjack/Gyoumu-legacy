<?php
namespace Oerp\Delivery\Page\Callee\Download_Sent;

class No__track extends \Oerp\Delivery\Page\Callee\Download_Sent\Abstract_fromView
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getQueryParam()
  {
    $param = parent::getQueryParam();
    $param['oerp:criteria'][] = array('x_tnum', 'in', array('', false));
    return $param;
  }

  protected function getEleOverrides()
  {
    return array(
      '#value' => 'Download No-track',
    );
  }
}