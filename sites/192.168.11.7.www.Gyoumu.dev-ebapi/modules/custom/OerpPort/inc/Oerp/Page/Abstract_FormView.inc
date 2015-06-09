<?php
namespace Oerp\Page;

abstract class Abstract_FormView extends \Gulei\Page\Form\_Abstract
{
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

  protected function getToolbarBottom() {
    $calls = array(__CLASS__ . "\\Callee\\Save");
    return $this->getLocalCalleeEle($calls);
  }
}