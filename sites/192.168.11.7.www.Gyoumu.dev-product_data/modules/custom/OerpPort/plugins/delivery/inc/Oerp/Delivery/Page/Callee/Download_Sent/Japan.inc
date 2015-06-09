<?php
namespace Oerp\Delivery\Page\Callee\Download_Sent;

class Japan extends \Oerp\Delivery\Page\Callee\Download_Sent\Abstract_fromView
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getQueryParam()
  {
    $filter = '
<filter>
  <func>
    <src><![CDATA[
    return $rec[$name] == "Japan";
    ]]></src>
    <fld>address_id.state_id.country_id</fld>
  </func>
</filter>
';
    $param = parent::getQueryParam();
    $param['filter'] = $filter;
    return $param;
  }

  protected function getEleOverrides()
  {
    return array(
      '#value' => 'Download Japan',
    );
  }
}