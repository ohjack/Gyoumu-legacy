<?php
namespace Rakuten\Importer;

class TrackingPostProc extends \Gulei\Importer\Arch\_Abstract {
  const MAIL_TEMPLATE_FIELD = 'mail_template_id';

  protected function getClassname() {
    return __CLASS__;
  }

  public function getArch() {
    $arch = <<<XML
<tree>
  <field name="O.ID"/>
  <field name="Track #"/>
</tree>
XML;
    return $arch;
  }

  public function submit($form, &$form_state) {
    $State = new \Gulei\Form\State($form_state);
    $src = $State->getVal('page_src');

    $pat = '@<td class="orderNo">[\s\S]*<a [\s\S]*>([\s\S]*)</a>[\s\S]*<input type="checkbox" name="order_number"@U';
    preg_match_all($pat, $src, $m);
    $scratches = array_combine($m[1], $m[0]);

    $recs = $this->getRecs($form);
    $mb_sig = 'https://image.rakuten.co.jp/com/img/rms/icon/mobile.gif';

    foreach ($recs as $ind => &$rec) {
      $rec['O.ID'] = $oid = trim($rec['O.ID']);
      $scratch = $scratches[$oid];

      if (!$scratch) {
        unset($recs[$ind]);
        continue;
      }

      if (strpos($scratch, $mb_sig) === FALSE) {
        $rec[self::MAIL_TEMPLATE_FIELD] = '-1';
      }
      else {
        $rec[self::MAIL_TEMPLATE_FIELD] = '-2';
      }
    }

    $recs = array_values($recs);
    foreach ($recs as $ind => &$rec) {
      unset($rec['%count']);
    }

    $hds = array_keys($recs[0]);
    $hds = array_combine($hds, $hds);

    $Tb = new \Gulei\Table\Simple($hds, $recs);
    $fpath = $Tb->getCsvFile(NULL, 'UploadTracking(rakuten)_fixed');
    $form_state['storage']['redirect'] = $fpath;
  }

  public function getEles($form_state) {
    $State = new \Gulei\Form\State($form_state);

    $form = array();
    $form['page_src'] = array(
      '#type' => 'textarea',
      '#title' => 'Page Source',
      '#default_value' => $State->getVal('page_src'),
    );
    return $form;
  }
}