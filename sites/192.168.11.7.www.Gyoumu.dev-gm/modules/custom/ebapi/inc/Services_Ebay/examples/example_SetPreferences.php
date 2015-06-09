<?PHP
/**
 * example that fetches shipping rates
 *
 * $Id: example_SetPreferences.php,v 1.1 2004/10/28 17:14:53 schst Exp $
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

$prefs = Services_Ebay::loadModel('Preferences', 'CrossPromotion');
$prefs->AddPreference('Enabled', 1, 'Boolean');

$sorting = Services_Ebay::loadModel('Preferences', 'Sorting');
$prefs->AddPreference($sorting);

$crosssell_sortfiltering = Services_Ebay::loadModel('Preferences', 'Crosssell_SortFiltering');
$crosssell_sortfiltering->AddPreference('BuyItNowSortFiltering', 1, 'Integer');
$crosssell_sortfiltering->AddPreference('FinalSortFiltering', 1, 'Integer');
$sorting->AddPreference($crosssell_sortfiltering);

$result = $ebay->SetPreferences($prefs);
if ($result === true) {
	echo 'Preferences set.';
}
?>