<?php
namespace Oerp\Delivery\Page\Callee\Download_Ready;

class No__track extends \Oerp\Delivery\Page\Callee\Abstract_Download
{
  protected function getArch() {
    return file_get_contents(__DIR__ . '/No__track.xml');
  }

  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getQueryParam()
  {
    $close_time = date('Y-m-d H:i:s', mktime(0, 0, -1));
    return array(
      'oerp:criteria' => array(
        array('state', '=', 'assigned'),
        array('x_tnum', 'in', array('', false)),
        array('date', '<=', $close_time),
        array('x_seller', '<>', FALSE),
      ),
    );
  }

  protected function getEleOverrides()
  {
    return array(
      '#value' => 'Download No-track',
    );
  }
}