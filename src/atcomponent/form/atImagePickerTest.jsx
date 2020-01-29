import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtImagePicker } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtImagePickerTest extends Component {
    constructor() {
        super(...arguments)

        this.state = {
            files: [{
                url: 'https://storage.360buyimg.com/mtd/home/111543234387022.jpg'
            },
            {
                url: 'https://storage.360buyimg.com/mtd/home/221543234387016.jpg'
            },
            {
                url: 'https://storage.360buyimg.com/mtd/home/331543234387025.jpg'
            }]

        }
    }

    onChange(files) {
        this.setState({ files })
    }

    onFail(msg) {
        console.log("onFail msg:", msg)
    }

    onImageClick(index, file) {
        console.log("index:", index, " file:", file)
    }

    render() {
        return (
            <View>
                <AtImagePicker
                    multiple={true}
                    length={5} // 单行数量
                    count={10} // 总数
                    files={this.state.files}
                    onChange={this.onChange.bind(this)}
                    onFail={this.onFail.bind(this)}
                    onImageClick={this.onImageClick.bind(this)}
                />
            </View>
        )
    }
}