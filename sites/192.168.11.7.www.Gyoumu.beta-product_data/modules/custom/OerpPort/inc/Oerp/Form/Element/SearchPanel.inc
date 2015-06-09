<?php
namespace Oerp\Form\Element;

class SearchPanel
{
  static public function process($eles, $edit, $form_state, $complete_form)
  {
    $flds = $eles['#eles'];
    unset($eles['#eles']);

    $key = implode('-', $eles['#parents']);
    $key = str_replace('_', '-', $key);
    $key .= $key ? '-' : '';

    $flds['func:find'] = array(
      '#type' => 'submit',
      '#value' => 'Find',
      '#name' => 'func:find',
      "#prefix" => '<div class="form-item" id="edit-' . $key . 'func:find-wrapper">',
      '#suffix' => '</div>',
    );

    $flds['func:clear'] = array(
      '#type' => 'submit',
      '#value' => 'Clear',
      '#name' => 'func:clear',
      "#prefix" => '<div class="form-item" id="edit-' . $key . 'func:clear-wrapper">',
      '#suffix' => '</div>',
    );

    $eles = array_merge($eles, $flds);
    $eles['#build_id'] = $complete_form['#build_id'];

    return $eles;
  }

  static public function theme($eles)
  {
    if (!$eles['#fields']) {
      return '';
    }

    $arch = '';
    foreach ($eles['#fields'] as $name => $fld) {
      switch ($fld['type']) {
        case 'float':
        case 'date':
        case 'datetime':
          $arch .= sprintf('
            <group colspan="1" col="4">
              <field name="%s-GE"/>
              <field name="%s-LE"/>
            </group>
          ', $name, $name);
          break;

        default:
          $arch .= '<field name="' . $name . '" colspan="1"/>';
      }
    }
    $arch .= '<newline/>
      <group colspan="1" col="4">
        <field type="submit" name="func:find"/>
        <field type="submit" name="func:clear"/>
      </group>
    ';
    $arch = '<form>' . $arch . '</form>';

    $key = implode('-', $eles['#parents']);
    $key = str_replace('_', '-', $key);

    $w = new \Oerp\View\Wrapper($eles['#fields'], $arch, $key, 5);
    return sprintf('<div class="searchpanel">%s</div>',
                   $w->renderForm($eles['#children'], $eles['#build_id']));
  }

  static public function submit($form, &$form_state, $eleSearch, $search)
  {
    $eleSearch = $eleSearch['_search'];
    $search = $search['_search'];
    $btn = $form_state['clicked_button']['#name'];

    switch ($btn) {
      case 'func:find':
        $cri = self::encodeCri(
          $search,
          $eleSearch['#fields']
        );

        $get = $_GET;
        $get['cri'] = $cri;
        unset($get['q'], $get['page']);
        $form_state['redirect'] = array($_GET['q'], http_build_query($get, '', '&'));

        return false;

      case 'func:clear':
        $form_state['redirect'] = array($_GET['q']);
        return false;
    }
    return true;
  }

  static public function encodeCri($search, $def)
  {
    if (!is_array($search)) {
      return array();
    }

    $cri = array();

    foreach ($search as $name => $kval) {
      if (strpos($name, 'func:') === 0) {
        continue;
      }

      switch ($def[$name]['type']) {
        case 'char':
        case 'many2one':
        case 'one2many':
          if (strlen($kval)) {
            $cri[] = array($name, 'ilike', rawurlencode($kval));
          }
          break;

        case 'int':
          $cri[] = array($name, '=', (int)$kval);
          break;

        case 'float':
          $min = (float)$kval['GE'];
          $max = (float)$kval['LE'];

          if ($min == $max) {
            if ($min != 0) {
              $cri[] = array($name, '=', $min);
            }
          }
          else {
            $cri[] = array($name, '>=', $min);
            $cri[] = array($name, '<=', $max);
          }
          break;

        case 'date':
        case 'datetime':
          if ($kval['GE']) {
            $cri[] = array($name, '>=', $kval['GE']);
          }

          if ($kval['LE']) {
            $cri[] = array($name, '<=', $kval['LE']);
          }
          break;

        case 'boolean':
          switch ($kval) {
            case 'yes':
              $cri[] = array($name, '=', 1);
              break;

            case 'no':
              $cri[] = array($name, '=', 0);
              break;
          }
          break;

        case 'selection':
          if ($kval == '_null_') {
            continue;
          }
          $cri[] = array($name, '=', $kval);
          break;
      }
    }
    return json_encode($cri);
  }

  static public function decodeCri($str, $asAssoc = false)
  {
    $cri = json_decode($str);

    if (!is_array($cri)) {
      return array();
    }

    foreach($cri as &$each){
      if(is_string($each[2])){
        $each[2] = rawurldecode($each[2]);
      }
    }

    if ($asAssoc) {
      $assoc = array();

      foreach ($cri as $each) {
        $assoc[$each[0]][$each[1]] = $each[2];
      }
      return $assoc;
    }
    else {
      return $cri;
    }
  }
}
