<?php
namespace Oerp\Page\FormView\Callee;

class Save extends \Gulei\Page\Form\Callee\_Abstract
{
  protected function getClassname() {
    return __CLASS__;
  }

  protected function saveForm($form, $form_state) {
    $param = $form['view']['#param'];

    //render nested value
    $post = $form_state['clicked_button']['#post']['view'];
    $nested = array_filter($post, 'is_array');

    $nested = array_map(
      create_function(
        '$n',
        'return \Oerp\Util\Helper::jsondecode($n["json"]);'
      ),
      $nested
    );

    $nested = \Oerp\Util\Helper::renderNestedInput($nested);

    //render form value
    $flds = $param['fields'];
    $vals = $form_state['values']['view'];

    foreach ($vals as $key => $val) {
      if ($flds[$key]['readonly']) {
        unset($vals[$key]);
      }
      elseif ($type = $flds[$key]['type']) {
        $args = array(
          'field' => array('type' => $type),
          'value' => $val,
          'nested_value' => $nested[$key],
        );

        $val = \Oerp\Form\Element\Factory::prepareSend($args);

        if (is_null($val)) {
          unset($vals[$key]);
        }
        else {
          $vals[$key] = $val;
        }
      }
      else {
        unset($vals[$key]);
      }
    }

    $q = new \Oerp\Query\Basic(
      $param['model'], NULL, NULL, array($param['rid']));

    if ($param['rid'] == 0) {
      $resp = $q->create($vals);
    }
    else {
      $resp = $q->write($vals);
    }

    if ($resp === TRUE) {
      return $param['rid'];
    }
    else {
      return $resp;
    }
  }

  public function execute($form, &$form_state) {
    $rid = $this->saveForm($form, $form_state);

    if (!$rid) {
      drupal_set_message("Failed to process the record.", 'error');
      return;
    }

    $entries = explode('/', $_GET['q']);
    array_pop($entries);

    $entries[] = $rid;
    $form_state['redirect'] = implode($entries, '/');

    $State = new \Gulei\Form\State($form_state);
    $btn = $State->getClickedButton();
    $entry_name = 'ns:' . str_replace("\\", ':', __CLASS__);

    if ($btn['#name'] == $entry_name) {
      drupal_set_message('Record saved.');
    }
    else {
      $param = $form['view']['#param'];
      $q = new \Oerp\Query\Basic($param['model']);

      switch ($btn['#param']['type']) {
        case 'object':
          $q->exec(
            array(
              $btn['#name'],
              array($rid),
            )
          );
          break;

        default:
          $q->exec_workflow(
            array(
              $btn['#name'],
              $rid,
            )
          );
          break;
      }
    }
  }
}