<?PHP
/**
 * example that fetches categories
 *
 * $Id: example_GetCategories.php,v 1.1 2004/10/28 17:14:53 schst Exp $
 *
 * @package     Services_Ebay
 * @subpackage  Examples
 * @author      Stephan Schmidt
 */
error_reporting(E_ALL);
require_once '../Ebay.php';
require_once 'config.php';

$session = Services_Ebay::getSession($devId, $appId, $certId);

$session->setToken($token);

$ebay = new Services_Ebay($session);

$cats = $ebay->GetCategories(12576);
echo '<pre>';
print_r($cats);
echo '</pre>';

/**
 * change the overall Detail-Level
 */
$session->setDetailLevel(1);

$cats = $ebay->GetCategories(12576);
echo '<pre>';
print_r($cats);
echo '</pre>';

?>