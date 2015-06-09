<?php
namespace Oerp\Delivery\Page\RakutenTracking\Callee;

class Download extends \Gulei\Page\Form\Callee\_Abstract {
  const MAIL_TEMPLATE_FIELD = 'mail_template_id';

  protected function getClassname() {
    return __CLASS__;
  }

  protected function getRecs($date) {
    $date_done = sprintf(
      '%s-%02s-%02s', $date['year'], $date['month'], $date['day']);

    $arch = <<<XML
<tree>
  <param name="oerp:model">stock.picking</param>
  <field name="origin"/>
  <field name="x_tnum"/>
  <field name="carrier_id">
    <field name="name"/>
  </field>
</tree>
XML;

    $cri = array(
      array('state', '=', 'done'),
      array('x_seller', 'ilike', 'titosoy'),
      array('date_done', '>=', $date_done . ' 00:00:00'),
      array('date_done', '<=', $date_done . ' 23:59:59'),
    );

    $q = new \Oerp\Query\Nested($arch, array('oerp:criteria' => $cri));
    return $q->tabulate();
  }

  protected function setMessage(&$rec) {
    static $tpls = NULL;

    if(is_null($tpls)) {
      $src = file_get_contents(__DIR__ . '/../reply_tpl.txt');
      preg_match_all('/#(.*) >>>([\s\S]*)<<</U', $src, $m);
      $tpls = array_combine($m[1], $m[2]);
    }

    $carr = $rec['carrier_id.name'];
    $rec['message'] = str_replace('<tracking_no>', $rec['x_tnum'], $tpls[$carr]);
  }

  public function execute($form, &$form_state) {
    $StateCtrl = new \Gulei\Form\State($form_state);
    $src = $StateCtrl->getVal('page_src');

    $pat = '@<td class="orderNo">[\s\S]*<a [\s\S]*>([\s\S]*)</a>[\s\S]*<input type="checkbox" name="order_number"@U';
    preg_match_all($pat, $src, $m);
    $scratches = array_combine($m[1], $m[0]);

    $recs = $this->getRecs($StateCtrl->getVal('date'));
    $mb_sig = 'https://image.rakuten.co.jp/com/img/rms/icon/mobile.gif';

    $recs_send = array_fill_keys(array_keys($scratches), FALSE);

    foreach ($recs as &$rec) {
      $rec['origin'] = $oid = trim($rec['origin']);
      $scratch = $scratches[$oid];

      if (!$scratch) {
        continue;
      }

      if (strpos($scratch, $mb_sig) === FALSE) {
        $rec[self::MAIL_TEMPLATE_FIELD] = '-1';
      }
      else {
        $rec[self::MAIL_TEMPLATE_FIELD] = '-2';
      }

      $this->setMessage($rec);
      $recs_send[$oid] = $rec;
    }

    $recs_send = array_filter($recs_send);
    $recs_send = array_values($recs_send);

    $hds = array(
      'origin' => 'O.ID',
      'x_tnum' => 'Track #',
      'message' => 'Message',
      self::MAIL_TEMPLATE_FIELD => 'Method',
    );

    $Tb = new \Gulei\Table\Simple($hds, $recs_send);

    $form_state['storage']['redirect']
        = $Tb->getCsvFile(null, 'UploadTracking(rakuten)');
  }
}