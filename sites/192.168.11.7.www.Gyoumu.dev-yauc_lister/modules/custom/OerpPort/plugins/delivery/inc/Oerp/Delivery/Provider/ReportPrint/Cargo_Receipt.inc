<?php
namespace Oerp\Delivery\Provider\ReportPrint;

class Cargo_Receipt extends \Oerp\Delivery\Provider\ReportPrint\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  public function getThemeSrc() {
    $Tb = $this->getTable();
    $tpl = <<<HTML
<div style="width: 700px;">
<h1 style="display:inline;">貿隆有限公司</h1>
325桃園龍潭鄉烏林村中豐路779巷10號

<p><strong>電話</strong> 03-4801776, 03-4801778</p>
<p><strong>傳真</strong> 03-4700828</p>

<p style="text-align:right"><strong>日期</strong> %s</p>
%s
<p style="text-align: right;"><strong>件數：</strong> %d</p>
</div>
HTML;

    return sprintf($tpl, date('Y-m-d'), $Tb->theme(), $Tb->getRowCount());
  }
}