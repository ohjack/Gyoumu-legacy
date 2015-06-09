<?php

function oerp_formview_process($ele, $edit, &$form_state, $form)
{
  $ctrl = new \Oerp\View\Form($ele);
  return $ctrl->process($ele, $edit, $form_state, $form);
}

function theme_oerp_formview($ele)
{
  $ctrl = new \Oerp\View\Form($ele);
  return $ctrl->theme();
}

function oerp_treeview_process($eles, $edit, &$form_state, $complete_form)
{
  $v = new \Oerp\View\Tree($eles);
  $eles = $v->processElement($eles);

  if ($eles['#param']['searchpanel']) {
    $eles['_search'] = $v->getSearchEntry();
  }
  return $eles;
}

function theme_oerp_treeview($eles)
{
  $v = $eles['#obj']->renew();
  return $v->theme($eles);
}


function oerp_many2one_process($eles, $edit, &$form_state, $complete_form)
{
  $attrs = $eles['#attributes'];

  $edit_class = ($attrs['readonly'] || !$eles['#value']['id'] || $attrs['selectonly'])
      ? 'ui-state-disabled' : '';

  $search_class = ($attrs['readonly'])
      ? 'ui-state-disabled' : '';

  $eles['rid'] = array(
    '#type' => 'hidden',
  );

  $eles['name'] = array(
    '#type' => 'textfield',
    '#title' => $eles['#title'],
    '#value' => $eles['#value']['name'],
    '#description' => $eles['#description'],
    '#required' => $eles['#required'],

    '#attributes' => array(
      'readonly' => '1',
      'selectonly' => $attrs['selectonly'],
      'class' => 'with_op',
      'rid' => $eles['#value']['id'],
      'model' => $eles['#value']['model'],
      'prepareSend' => $attrs['prepareSend'],
    ),

    '#field_suffix' => sprintf('
      <div class="oerp-button oerp-btn-edit %s">
        <span class="oerp-icon ui-icon ui-icon-pencil"></span>
      </div>
      <div class="oerp-button oerp-btn-search %s">
        <span class="oerp-icon ui-icon ui-icon-search"></span>
      </div>
    ', $edit_class, $search_class),
  );

  foreach ($eles['#attributes'] as $attr_name => $attr_val) {
    $eles['name']['#attributes'][$attr_name] = $attr_val;
  }
  return $eles;
}

function theme_oerp_many2one($eles)
{
  preg_match('@<div[^>]*>([\s\S]*)</div>@', $eles['#children'], $m);

  return sprintf('
    <div id="%s-wrapper" class="form-item awakable" data-awake="OerpMany2oneWidget">%s</div>
  ', $eles['#id'], $m[1]);
}

function oerp_searchpanel_process($eles, $edit, $form_state, $complete_form)
{
  return \Oerp\Form\Element\SearchPanel::process($eles, $edit, $form_state, $complete_form);
}

function theme_oerp_searchpanel($eles)
{
  return \Oerp\Form\Element\SearchPanel::theme($eles);
}


function oerp_ViewForm_form(&$form_state, $model, $vid, $rid, $param = array())
{
  $ctrl = new \Oerp\Page\FormView($form_state);
  return $ctrl->hook_form(func_get_args());
}

function oerp_ViewForm_form_submit($form, &$form_state)
{
  $ctrl = new \Oerp\Page\FormView($form_state);
  return $ctrl->hook_form_submit($form, &$form_state);
}

function oerp_ViewEditor_js()
{
  $prompt = new \Oerp\View\Prompt();
  return $prompt->execute();
}

function oerp_ViewTree_form(&$form_state, $vid, $param = array())
{
  $p = new \Oerp\Page\TreeView($form_state);
  return $p->hook_form(func_get_args());
}

function oerp_ViewTree_form_submit($form, &$form_state)
{
  $p = new \Oerp\Page\TreeView($form_state);
  $p->hook_form_submit($form, $form_state);
}

function oerp_execute_js() {
  $hooks = \Oerp\Hook::searchJsHook();

  foreach ($hooks as $hook) {
    if ($hook->applicable()) {
      return $hook->exe();
    }
  }
  return '';
}

function oerp_oerp_js_hook() {
  return array(
    new \Oerp\Hook\JS\Wizard(),
    new \Oerp\Hook\JS\Workflow(),
    new \Oerp\Hook\JS\ExeCreate(),
    new \Oerp\Hook\JS\ExeWrite(),
    new \Oerp\Hook\JS\ExeGetName(),
    new \Oerp\Hook\JS\OnChange(),
    new \Oerp\Hook\JS\ExeGeneral(),
  );
}