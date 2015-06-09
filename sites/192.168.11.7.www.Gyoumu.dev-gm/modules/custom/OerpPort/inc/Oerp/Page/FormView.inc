<?php
namespace Oerp\Page;

class FormView extends \Gulei\Page\Form\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    list($model, $vid, $rid, $param_base) = $this->args;

    $param = array(
      'model' => $model,
      'vid' => $vid,
      'rid' => $rid,
    );

    $param = is_array($param_base) ? array_merge($param_base, $param) : $param;
    return $param;
  }

  public function hook_form_internal()
  {
    $param = $this->getParam();

    if ($param['model'] && !strpos($param['rid'], '0-') === 0) {
      $q = new \Oerp\Query\Basic($param['model'], null, array('id'), $param['rid']);
      if (!(array)$q->raw()) {
        drupal_set_message('Specified record is not found.', 'error');

        return drupal_goto(
          sprintf(
            'oerpport/view/form/%s/%s/0',
            $param['model'], $param['rid']
          )
        );
      }
    }

    $form = array();
    $form['#attributes'] = array('class' => 'oerp-formview');
    $form['view'] = array(
      '#type' => 'oerp_formview',
      '#param' => $param,
    );
    return $form;
  }

  protected function getToolbarBottom()
  {
    $form = array();
    $form['func:submit'] = array(
      '#type' => 'submit',
      '#value' => 'Save',
      '#name' => 'func:submit',
    );
    return $form;
  }

  private function saveForm($form, $form_state)
  {
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
      else if($type = $flds[$key]['type']){
        $args = array(
          'field' => array('type' => $type),
          'value' => $val,
          'nested_value' => $nested[$key]
        );

        $val = \Oerp\Form\Element\Factory::prepareSend($args);

        if(is_null($val)) {
          unset($vals[$key]);
        }
        else {
          $vals[$key] = $val;
        }
      }
      else{
        unset($vals[$key]);
      }
    }

    $q = new \Oerp\Query\Basic($param['model'], null, null, array($param['rid']));

    if ($param['rid'] == 0) {
      $resp = $q->create($vals);
    }
    else {
      $resp = $q->write($vals);
    }

    if ($resp === true) {
      return $param['rid'];
    }
    else {
      return $resp;
    }
  }

  protected function proc_func_submit($form, &$form_state)
  {
    $rid = $this->saveForm($form, $form_state);

    if (!$rid) {
      drupal_set_message("Failed to process the record.", 'error');
      return;
    }

    drupal_set_message('Record saved.');

    $entries = explode('/', $this->q);
    array_pop($entries);

    $entries[] = $rid;
    $form_state['redirect'] = implode($entries, '/');
  }
}