import json
import urllib.request
import urllib.parse
import urllib.error
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-c', '--city')
parser.add_argument('-s', '--state')
parser.add_argument('-C', '--country')
args = parser.parse_args()

city = args.city
state = args.state
country = args.country
apikey = '86b0bd9b76517148d71f0967cc7de574'
params = {
    'q': f'{city},{state},{country}',
    'appid': apikey
}
query_string = urllib.parse.urlencode(params)
url = 'https://api.openweathermap.org/data/2.5/weather' + '?' + query_string
try:
    row_response = urllib.request.urlopen(url)
    json_response = row_response.read().decode('utf-8')
    response = json.loads(json_response)
    print('Coordination:')
    print('    Longitude of the location: ', response['coord']['lon'])
    print('    Latitude of the location: ', response['coord']['lat'])
    print('Weather information:')
    print('    main weather: ', response['weather'][0]['main'])
    print('    description: ', response['weather'][0]['description'])
    print('Main information:')
    print('    Temperature: ', int(response['main']['temp'] - 273), '°C')
    print('    Minimum Temperature: ', int(response['main']['temp_min'] - 273), '°C')
    print('    Maximum Temperature: ', int(response['main']['temp_max'] - 273), '°C')
    print('    Atmospheric pressure: ', response['main']['pressure'], 'Pa')
    print('    Humidity: ', response['main']['humidity'], '%')
    print('    Visibility: ', response['visibility'], 'm')
    print('Wind information:')
    print('    Wind speed: ', response['wind']['speed'], 'm/s')
    print('    Wind direction: ', response['wind']['deg'], 'deg')
    print('Other information:')
    print('    Cloudiness: ', response['clouds']['all'], '%')
    print('Location information:')
    print('    City: ', response['name'])
    print('    Country: ', response['sys']['country'])

except urllib.error.HTTPError as e:
    print('HTTPError: ', e.code, e.reason)
except urllib.error.URLError as e:
    print('URLError: ', e.reason)
