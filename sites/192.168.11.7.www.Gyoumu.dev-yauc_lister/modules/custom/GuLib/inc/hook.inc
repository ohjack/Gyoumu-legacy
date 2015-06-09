<?php

function GuLib_GuSelectTable_process($ele, $edit, &$form_state, $form){
  $obj = $ele['#obj'];
  $obj->renew();

  $ele = array_merge($ele, $obj->getSelectionEles($ele));
  return $ele;
}

function theme_GuSelectTable($eles){
  drupal_add_js(drupal_get_path('module', 'GuLib') . '/js/GuSelectTable.js');
  $obj = $eles['#obj']->renew();

  $gu_name = $eles['#parents'][count($eles['#parents']) - 1];

  return sprintf('<div gu_name="%s">%s</div>',
    $gu_name, $obj->theme($eles));
}

function GuLib_GuForm_process($ele)
{
  $ele['#tree'] = true;
  $obj = $ele['#obj']->renew();

  return array_merge($obj->getFormEles(), $ele);
}

function theme_GuForm($form)
{
  \Gulei\Form\Basic::addCss();
  \Gulei\Form\Basic::addJs();

  $obj = $form['#obj']->renew();

  $output = sprintf('<div class="gu-view gu-block-%s">%s</div>',
                    $form['#width'], $obj->theme($form));

  return html_entity_decode($output);
}