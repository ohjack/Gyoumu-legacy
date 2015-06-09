<?php
namespace Oerp\Purchase\Page;

class PickingMatch extends \Gulei\Page\Form\_Abstract {
  protected $inited;

  protected function getClassname() {
    return __CLASS__;
  }

  public function getPo() {
    $param = array(
      'no-selection-name' => FALSE,
    );

    $prov = new \Oerp\Purchase\Provider\PoLines($_GET['rid'], $param);
    $po = $prov->getData();
    return $po[0];
  }

  protected function init() {
    if ($this->inited) {
      return;
    }

    $this->po = $this->getPo();
    $this->pick = $this->getPick();

    jquery_ui_add(array('ui.draggable', 'ui.dialog'));

    \Gulei\Includer\JS\Datepicker::inc();

    \Gulei\Util\Path::add_js(
      __DIR__ . '/PickingMatch/PickingMatch.js');

    \Gulei\Util\Path::add_css(
      __DIR__ . '/PickingMatch/PickingMatch.css');

    $this->inited = TRUE;
  }

  protected function getPick() {
    $prov = new \Oerp\Purchase\Provider\PoPicking($_GET['rid']);
    $picks = $prov->getData();
    return array_pop($picks);
  }

  protected function getArch() {
    return file_get_contents(__DIR__ . '/PickingMatch.xml');
  }

  protected function getArchFields() {
    $Ele = new \Gulei\Arch\Element($this->getArch());
    return array_keys($Ele->extractAttr('name'));
  }

  protected function getMoveHistory($sku) {
    $moves = $this->po['order_line'][$sku]['move_ids'];

    foreach ($moves as $ind => &$move) {
      if ($move['product_qty'] < 1) {
        unset($moves[$ind]);
      }

      if (strpos($move['note'], '[RETURN]') === 0) {
        $move['product_qty'] *= -1;
      }
    }

    $moves = array_values($moves);

    $arch = <<<XML
<tree>
  <field name="date_planned" string="Pick Date"/>
  <field name="product_qty" string="Qty"/>
  <field name="product_uom" string="UoM"/>
  <field name="note" string="Note"/>
  <field name="state" string="Status"/>

  <translate>
    <func>
      <src name="cmd:explode">
        <arg> </arg>
        <arg>0</arg>
      </src>
      <fld>date_planned</fld>
    </func>
  </translate>
</tree>
XML;

    $param = array(
      'mark_translated' => FALSE,
    );

    $TbMoves = new \Gulei\Table\Arch(
      $arch, new \Gulei\Provider\Lambda($moves), $param);

    return sprintf(
      '<div class="move-history sku-%s">%s</div>', $sku, $TbMoves->theme());
  }

  public function getTableRecs() {
    $this->init();

    $Lines = new \Oerp\Util\_Array(array_values($this->po['order_line']));
    $lines = array();
    $Lines->inherit($lines);

    foreach ($lines as $ind => &$line) {
      if (!$line['id'] || $line['product_qty'] < 1) {
        unset($lines[$ind]);
        continue;
      }

      $line['product_id.name'] = sprintf(
        '<span><span>%s</span> <a href="#" sku="%s">history</a></span>',
        $line['product_id.name'],
        $line['product_id.default_code']
      );
      $sku = $line['product_id.default_code'];

      if (!$got = $this->pick[$sku]['done']) {
        $got = 0;
      }

      $line['func:picked'] = '/ ' . $got;
    }
    return $lines;
  }

  protected function getTableSrc() {
    $recs = $this->getTableRecs();
    $arch = $this->getArch();

    $Table = new \Gulei\Table\Arch(
      $arch,
      new \Gulei\Provider\Lambda($recs)
    );

    return $Table->theme();
  }

  protected function getDialogTpl($id, $dvals) {
    $dia = array(
      '#prefix' => '<div id="' . $id . '" style="display:none;">',
      '#suffix' => '</div>',
    );

    foreach ($this->po['order_line'] as $sku => $line) {
      if ($line['product_qty'] < 1) {
        continue;
      }

      $prod = $line['product_id'];
      $dval = $dvals[$sku];
      $dval = $dval ? $dval : '0';
      $date = date('Y-m-d', time());

      $title = sprintf('<h2>%s<span class="prod_name"> - %s</span></h2>', $prod['default_code'], $prod['name']);

      $dia[$sku] = array(
        '#prefix' => '<div class="wrapper-line">' . $title,
        '#suffix' => '</div>',
      );

      $order_qty = $line['product_qty'];

      $dia[$sku]['pick_qty'] = array(
        '#title' => 'Qty',
        '#type' => 'textfield',
        '#default_value' => 0,
        '#size' => '',
        '#attributes' => array(
          'dval' => 0,
          'order_qty' => $order_qty,
          'picked' => $dval,
        ),
      );

      $dia[$sku]['pick_date'] = array(
        '#title' => 'Pick Date',
        '#type' => 'textfield',
        '#size' => '',
        '#attributes' => array('widget' => 'datepicker'),
        '#default_value' => $date,
      );

      $dia[$sku]['pick_note'] = array(
        '#title' => 'Note',
        '#type' => 'textfield',
        '#size' => '',
      );

      $hint = sprintf(
        '<div class="qty_hint">Picked: <span class="qty_to">%s</span> / %s</div>', $dval, $order_qty);

      $dia[$sku]['pick_qty_hint'] = array(
        '#type' => 'markup',
        '#value' => $hint,
      );
    }
    return $dia;
  }

  protected function getPickDvals() {
    if (!$this->pick) {
      return array();
    }

    $dvals = array();
    foreach ($this->pick as $sku => $pick) {
      if (strpos($sku, '_') !== 0) {
        $dvals[$sku] = $pick['done'];
      }
    }
    return $dvals;
  }

  protected function isSubmitDisabled() {
    if ($this->po['state'] !== 'approved'
        && $this->po['state'] !== 'done') {
      return TRUE;
    }
    $pick_copy = array_values($this->pick);
    return !isset($pick_copy[1]['confirmed']);
  }

  protected function hook_form_internal() {
    $this->init();

    $src = '';
    $src .= '<div class="caption">' . $this->po['name'] . '</div>';
    $src .= '<h3>' . $this->po['partner_id']['name'] . '</h3>';
    $src .= $this->getTableSrc();

    $form = array(
      '#tree' => TRUE,
    );

    $form['info'] = array(
      '#type' => 'markup',
      '#value' => $src,
    );

    $form['btn-dia-pack'] = array(
      '#type' => 'button',
      '#value' => 'Pick / Return',
//      '#disabled' => $this->isSubmitDisabled() ? TRUE : FALSE,
      '#attributes' => array(
        'target-dialog' => 'dialog-pack',
      ),
    ) ;

    $form['callees'] = array_merge(
      array(
        '#prefix' => '<div class="hidden-callee">',
        '#suffix' => '</div>',
      ),
      $this->getLocalCalleeEle('Submit')
    );

    $form['send'] = array(
      '#type' => 'hidden',
    );

    $form['dialog-pack'] =
        $this->getDialogTpl('dialog-pack', $this->getPickDvals());

    $src_history_tables = '';
    foreach (array_keys($this->po['order_line']) as $sku) {
      $src_history_tables .= $this->getMoveHistory($sku);
    }

    $form['src_history'] = array(
      '#type' => 'markup',
      '#value' => $src_history_tables,
    );

    return $form;
  }

  protected function getToolbarBottom() {
    return array();
  }

  public function getParam() {
    return array();
  }
}