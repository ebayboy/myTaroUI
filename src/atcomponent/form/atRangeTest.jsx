import Taro from '@tarojs/taro'
import { View } from '@tarojs/components'
import { AtRange, AtToast } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

export default class AtRangeTest extends Taro.Component {
    constructor() {
        super(...arguments)
        this.state = {
            isOpened: false,
            text: '',
        }
    }
    handleChange(value) {
        this.setState({
            isOpened: true,
            text: `${value[0]}~${value[1]}`
        })
    }

    render() {
        return (
            <View>
                <AtToast
                    text={this.state.text}
                    isOpened={this.state.isOpened}
                />
                <AtRange
                    value={[20, 60]}
                    min={0}
                    max={100}
                    onChange={this.handleChange.bind(this)}
                />
            </View>
        )
    }
}