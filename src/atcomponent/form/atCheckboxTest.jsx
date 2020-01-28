import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtCheckbox } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtCheckboxTest extends Component {
    constructor() {
        super(...arguments)

        this.state = {
            options: [{
                value: 'list1',
                label: 'iPhone X',
                desc: '部分地区提供电子普通发票，用户可自行打印，效力等同纸质普通发票，具体以实际出具的发票类型为准。'
            }, {
                value: 'list2',
                label: 'HUAWEI P20'
            }, {
                value: 'list3',
                label: 'OPPO Find X',
                desc: '部分地区提供电子普通发票，用户可自行打印，效力等同纸质普通发票，具体以实际出具的发票类型为准。',
                disabled: true
            }, {
                value: 'list4',
                label: 'vivo NEX',
                desc: '部分地区提供电子普通发票，用户可自行打印，效力等同纸质普通发票，具体以实际出具的发票类型为准。',
                disabled: true
            }],
            selectedList: []
        }
    }

    handleClick(value) {
        this.setState({
            selectedList: value
        })

        console.log("select options:", this.state.selectedList)
    }

    render() {
        const options = this.state.options
        const selectedList = this.state.selectedList
        return (
            <View>
                <AtCheckbox
                    options={options}
                    selectedList={selectedList}
                    onChange={this.handleClick.bind(this)}
                />
            </View>
        )
    }
}