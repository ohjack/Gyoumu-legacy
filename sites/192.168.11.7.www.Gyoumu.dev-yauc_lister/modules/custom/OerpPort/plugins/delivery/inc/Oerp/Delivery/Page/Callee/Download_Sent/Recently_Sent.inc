<?php
namespace Oerp\Delivery\Page\ControlPanel\Callee\Download_Sent;

class Recently_Sent extends \Oerp\Delivery\Page\ControlPanel\Callee\Abstract_Download
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getQueryParam()
  {
    $pat = 'Y-m-d H:i:s';
    $start_time = date($pat, mktime(0, 0, 0, date('m'), date('d') - 4));

    return array(
      'oerp:criteria' => array(
        array('state', '=', 'done'),
        array('date_done', '>=', $start_time),
      ),
    );
  }
}