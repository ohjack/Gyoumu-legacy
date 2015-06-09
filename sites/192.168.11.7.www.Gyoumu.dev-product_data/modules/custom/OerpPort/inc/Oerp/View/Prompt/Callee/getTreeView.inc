<?php
namespace Oerp\View\Prompt\Callee;

class getTreeView extends _Abstract
{
  public function isApplicable()
  {
    return $this->post['type'] == 'tree';
  }

  public function execute()
  {
    $post = $this->post;

    if ($post['domain']) {
      $domain = strtr(
        $post['domain'],
        array(
          "'" => '"',
          '(' => '[',
          ')' => ']',
        )
      );

      $tmpDomain = json_decode($domain);
      $tmpDomain = is_array($tmpDomain) ? $tmpDomain : array();

      $cri = array();
      foreach ($tmpDomain as $each) {
        $cri[$each[0]][$each[1]] = $each[2];
      }
      unset($tmpDomain);

      foreach ($cri as $name => $each) {
        if (isset($each['>=']) && isset($each['<='])) {
          $min = (float)$each['>='];
          $max = (float)$each['<='];

          if ($min == $max) {
            if ($min == 0) {
              unset($cri[$name]);
            }
            else {
              $cri[$name] = array('=' => (string)$min);
            }
          }
        }
      }

      $domain = array();
      foreach ($cri as $name => $exp) {
        foreach ($exp as $op => $val) {
          if ($val != '_null_') {
            $domain[] = array($name, $op, $val);
          }
        }
      }
      $domain = json_encode($domain);
    }

    $param = array(
      'model' => $post['model'],
      'name' => $post['name'],
      'criteria' => $domain,
      'cri_url' => $domain,
      'oerp:op-type' => 'select-one',
      'prompt' => true,
      'pager' => true,
      'items_per_page' => 80,
      'pager_hide' => true,
      'hide' => array('toolbar-bottom'),
    );

    $src = drupal_get_form('oerp_ViewTree_form', $post['vid'], $param);
    $src = '<div style="max-height: 400px; overflow: auto;" class="awakable" data-awake="OerpTreeWidget">' . $src . '</div>';

    return drupal_json(rawurlencode($src));
  }
}